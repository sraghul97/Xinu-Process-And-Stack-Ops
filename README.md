# Xinu-Process-And-Stack-Ops

This repository contains the implementation of key features related to process management and stack handling in the Xinu operating system, as part of the ECE465/565 – Operating Systems Design course. The project focuses on two primary tasks:

Cascading Termination: Implementation of a cascading termination mechanism for user processes. This involves modifying Xinu to support the termination of user processes and all their descendants, distinguishing between system and user processes. The modification includes the addition of a user_process flag in the Process Control Block (PCB) and updating the process creation function to handle this flag appropriately. A test case file, main.kill, demonstrates the functionality by spawning and terminating multiple user processes.

Fork System Call: Development of a fork system call that mimics Unix's fork behavior. The new system call creates a child process that closely mirrors the parent process in terms of name, priority, and stack length. The child process is initialized and set to a READY state, and upon creation, it resumes execution from the instruction following the fork call. This feature is tested using provided test cases in main.fork, with outputs verified against expected results.

Additional tasks include a thorough exploration of Xinu’s process management architecture, stack handling, and the use of debugging tools to ensure the correctness of the implementation. The project also involves a detailed report answering key questions about the Xinu operating system’s process management and stack handling features.
