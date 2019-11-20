//
//  TestTaskflow.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 10/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//


#include "taskflow/taskflow.hpp"

int main_taskflow(){

  tf::Executor executor;
  tf::Taskflow taskflow;
  
  auto A = taskflow.placeholder();
   auto B = taskflow.placeholder();
   auto C = taskflow.placeholder();
   auto D = taskflow.placeholder();
   
   A.work([] () { std::cout << "TaskA\n"; });
   B.work([] () { std::cout << "TaskB\n"; });
   C.work([] () { std::cout << "TaskC\n"; });
   D.work([] () { std::cout << "TaskD\n"; });
   
  
 
                                           //  | A |           | D |
  A.precede(B);  // B runs after A         //  +---+           +-^-+
  A.precede(C);  // C runs after A         //    |     +---+     |
  B.precede(D);  // D runs after B         //    +---->| C |-----+
  C.precede(D);  // D runs after C         //          +---+

  executor.run(taskflow).wait();

  return 0;
}

