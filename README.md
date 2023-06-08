# Networks-Lab-2023
This repository contains all the assignments of the Computer Networks Laboratory Course (CS39006) offered at IIT Kharagpur in the Spring Semester of 2022-23 academic year. The course was taught by Prof. Arobinda Gupta and Prof. Sudip Misra.
- Assignment 1-3 are done individually by me.
- Assignment 4-6 are done in a group of 2. The group members are:
    - Abhay Kumar Keshari (20CS10001)
    - Jay Kumar Thakur (20CS30024)

## Prerequisites
- Solid understanding of the C programming language
- Familiarity with basic data structures and algorithms
- Familiarity with unix-based [MacOS/Linux] environment

## Getting Started
1. Clone the repository to your local machine:
``` 
git clone https://github.com/abhay-k47/Networks-Lab-2023.git
```

2. Navigate to the specific assignment folder to access the problem statement, relevant code, and instructions to run the code.

## Description

### Assignment 1: Iterative TCP client-server programming
- **Task**: The goal of this assignment is to practice TCP socket programming, iterative server design, and client-server communication. The assignment is divided into two parts:
    - Part 1: TCP iterative server for retrieving system date and time from the server.
    - Part 2: TCP iterative server and client for evaluating arithmetic expressions.

### Assignment 2: UDP and concurrent TCP client-server programming
- **Task**: The goal of this assignment is to practice and demonstrate  TCP and UDP client-server programming. It aims to develop skills in implementing socket communication protocols, handling timeouts, concurrent server design, user authentication, and executing shell commands within the context of network programming.
    - Part 1: UDP Client-Server System for Time Retrieval
    - Part 2: Client-Server Communication with User Authentication and Shell Commands

### Assignment 3: Client-Server Communication via Load Balancer
- **Task**: The goal of this assignment is to develop a basic load balancer system that effectively distributes client requests among multiple servers to achieve load balancing. It aims to enhance understanding of network programming, server-client communication, load monitoring, and load distribution strategies.

### Assignment 4: Simple HTTP Client-Server Implementation
- **Task**: The goal of this assignment is to provide a hands-on experience in implementing a simplified TCP concurrent HTTP client and server. It aims to gain a practical understanding of the HTTP protocol, including its commands, headers, and message formats, learn about client-server communication, handling requests and responses, and adhering to protocol specifications.

### Assignment 5: Message Oriented TCP Implementation
- **Task**: The goal of assignment is to implement a message-oriented TCP protocol called *MyTCP*. The objective is to create a custom socket type that guarantees reliable and in-order delivery of messages, similar to TCP, but with the behavior of sending and receiving a complete message with a single send or receive call. This involves creating functions such as my_socket, my_bind, my_accept, my_connect, my_listen, my_send, my_recv, and my_close that emulate the behavior of TCP sockets but ensure message-oriented communication. The implementation requires handling threads, message buffers, and proper synchronization. The assignment also includes designing message formats and tables for sending and receiving messages.

### Assignment 6: *pingnetinfo* - tool for network path discovery, latency estimation, and bandwidth estimation
- **Key Learning Objectives**
    - ICMP protocol and ICMP header fields
    - IP protocol and IP header fields
    - Programming with Raw sockets
    - Traceroute and Ping tools and their functionality
    - Latency and Bandwidth in networks


- **Task**: The goal of the this assignment is to develop a program that can probe a network by sending ICMP packets and gather information about the network path, latency, and bandwidth of each link in the path. The program aims to achieve the following:

    - **Network Path Discovery**: Discover the intermediate nodes or links between a source and destination by emulating the functionality of the traceroute tool. Identify each link in the network path and print out the corresponding IP addresses or names.

    - **Latency Estimation:** Measure and estimate the latency of each link by sending ICMP packets with a 0-length message. Calculate the time taken for a packet to travel from the source to each intermediate node, considering factors such as packet processing and transmission delays.

    - **Bandwidth Estimation**: Estimate the bandwidth of each link by sending ICMP packets with varying amounts of data. Measure the time difference between packets of different sizes to assess the processing time required for different data volumes, enabling the estimation of link bandwidth.

    - **Handling Network Variability**: Account for potential packet loss, out-of-order packet delivery, and unresponsive nodes in the network. Handle these scenarios gracefully to ensure the program does not hang or crash and continues to gather available information from the network.

    - **Informative Output**: Print out detailed header information of sent and received ICMP packets, including IP and protocol header fields. Provide a clear and well-formatted output to present the network path, latency, and bandwidth information for each link, as well as any additional relevant data extracted from the received packets.

    Overall, the aim is to create a robust and informative program that enables network analysis and assessment of latency and bandwidth characteristics of a given network path.

## Usage
1. Follow the instructions provided in each assignment  folder to understand the task and requirements.
2. Run the code using the specified command or execution method.
3. Provide the necessary inputs as prompted or modify the code as required.
4. Analyze the output or observe the network behavior as mentioned in the assignment guidelines.

## Troubleshooting
- If you encounter any issues while running the assignments, please feel free to contact us or raise an issue in this repository.

## References
- [Introduction to Sockets Programming in C using TCP/IP](https://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf)
- [Man-pages](https://man7.org/linux/man-pages/dir_all_alphabetic.html)
- [ICMP-Guide](http://www.tcpipguide.com/free/t_InternetControlMessageProtocolICMPICMPv4andICMPv6.htm)

## License
- The code in this repository is licensed under the [MIT License](https://choosealicense.com/licenses/mit/).

## Contributors
- Abhay Kumar Keshari (20CS10001) [[abhay-k47](https://github.com/abhay-k47)]
- Jay Kumar Thakur (20CS30024) [[jkt10125](https://github.com/jkt10125)]

