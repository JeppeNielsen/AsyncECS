//
//  QuadTree.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//
#pragma once
#include "BoundingBox.hpp"
#include "QuadTreeNode.hpp"
#include <vector>

namespace Game {
    template<typename T>
    class Quadtree {
    public:
        using Node = QuadtreeNode<T>;
        
        using Nodes = std::vector<Node*>;
        
        void GetRecursive(std::vector<T>& list) {
            if (!nodes) return;
            for (unsigned i=0;i<nodes->size(); i++) {
                list.push_back((T)nodes->operator[](i)->data);
            }
            if (!children) return;
            for(int i=0; i<4; i++) children[i].GetRecursive(list);
        }
        
        void Get(const BoundingBox& boundingBox, std::vector<T>& list) const {
            if (!box.Intersects(boundingBox)) {
                return;
            }

            for(auto node : nodes) {
                if (node->box.Intersects(boundingBox)) {
                    list.push_back(node->data);
                }
            }
            if (children.empty()) {
                return;
            }
            for (int i=0; i<4; i++) {
                children[i].Get(boundingBox, list);
            }
        }
        
        Quadtree() : parent(nullptr) { }

        Quadtree(const BoundingBox& box) : parent(nullptr) {
            SetBoundingBox(box);
        }

        void SetBoundingBox(const BoundingBox& box) {
            this->box = box;
        }

        bool Insert(Quadtree::Node& node) {
            bool test;
            if (parent) {
                test = box.Contains(node.box);
            } else {
                test = box.Intersects(node.box);
            }

            if (test) {
                if (nodes.size()>=MaxObjectsInNode) {
                    Split();
                    bool insertedIntoChildren = false;
                    for (int i=0; i<4; i++) {
                        insertedIntoChildren = children[i].Insert(node);
                        if (insertedIntoChildren) break;
                    }
                    if (!insertedIntoChildren) {
                        node.childIndex = nodes.size();
                        node.node = this;
                        nodes.push_back(&node);
                    }
                } else {
                    node.childIndex = nodes.size();
                    node.node = this;
                    nodes.push_back(&node);
                }
                return true;
            }

            return false;
        }

        void Remove(Node& node) {
            Nodes& nodes = node.node->nodes;
            
            if (nodes.size()<=1) {
                nodes.clear();
            } else {
                Node* last = nodes.back();
                nodes[node.childIndex] = last;
                last->childIndex = node.childIndex;
                nodes.pop_back();
            }
            
            node.node = nullptr;
        }

        void Move(Node& node) {
            if (node.node->box.Contains(node.box)) {
                return;
            } else {
                Remove(node);
                Insert(node);
            }
        }

        void Split() {
            if (!children.empty()) return;
            children.resize(4);

            Vector2 size = box.extends * 0.5f;
            Vector2 offset = size * 0.5f;

            children[0].box = BoundingBox(box.center + Vector2(-offset.x, -offset.y), size);
            children[1].box = BoundingBox(box.center + Vector2(-offset.x, offset.y), size);
            children[2].box = BoundingBox(box.center + Vector2(offset.x, offset.y), size);
            children[3].box = BoundingBox(box.center + Vector2(offset.x, -offset.y), size);

            for (int i=0; i<4; i++) {
                children[i].parent = this;
            }
        }
    public:
        const static unsigned MaxObjectsInNode = 32;
        BoundingBox box;
        Quadtree* parent;
        std::vector<Quadtree> children;
        Nodes nodes;
    };
}

