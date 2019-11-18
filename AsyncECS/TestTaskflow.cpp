//
//  TestTaskflow.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 10/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//


//#include "taskflow.hpp"
/*

int main(){

  tf::Executor executor;
  tf::Taskflow taskflow;
  
  auto [A, B, C, D] = taskflow.emplace(
    [] () { std::cout << "TaskA\n"; },     //
    [] () { std::cout << "TaskB\n"; },     //          +---+
    [] () { std::cout << "TaskC\n"; },     //    +---->| B |-----+
    [] () { std::cout << "TaskD\n"; }      //    |     +---+     |
  );                                       //  +---+           +-v-+
                                           //  | A |           | D |
  A.precede(B);  // B runs after A         //  +---+           +-^-+
  A.precede(C);  // C runs after A         //    |     +---+     |
  B.precede(D);  // D runs after B         //    +---->| C |-----+
  C.precede(D);  // D runs after C         //          +---+

  executor.run(taskflow).wait();

  return 0;
}
*/
