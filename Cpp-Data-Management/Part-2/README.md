Project Name: Accelerometer Data Processing Project

Overview:
This project involves the processing of data received from an embedded system's 3-axis accelerometer sensor. The objective is to extract meaningful information from the data stored in sample_data.txt which has data from multiple sensors. We need to extract accelerometer data including accelerometer readings and checksum validation, adhering to specific message formats and constraints.

Files Included:
- Acceleo.h: Header file containing the Acceleo class definition.
- Acceleo.cpp: Implementation file containing functions for processing accelerometer data.
- main.c: Main file handling data reading and interaction with the Acceleo class.
- sample_data.txt: Input file containing the sample data for processing.

Compilation and Execution:
To compile the project using GCC (GNU Compiler Collection), follow these steps:

1. Open a terminal or command prompt in the project directory.
2. Run the following command to compile the code:
   g++ main.cpp Acceleo.cpp -o parser 

3. Once compiled successfully, execute the program by entering:
   ./parser

Project Structure:
- Acceleo.h contains the class definition for the Acceleo class, encapsulating essential functions and variables for data processing.
- Acceleo.cpp implements the functions defined in Acceleo.h, handling accelerometer data parsing, validation, and output operations.
- main.c manages the main program flow, reading data from the sample_data.txt file, and interacting with the Acceleo class functions.

Usage:
1. The program reads data from the sample_data.txt file and processes it according to the defined accelerometer message structure.
2. Data processing includes extracting accelerometer readings (XXX, YYY, ZZZ), performing checksum validation, handling alerts, and recording statistics.
3. Processed data is displayed in the terminal as debug outputs and written to an output file named output.txt.
4. Checksum validation results, alert signals, and statistics for processed data are presented for each iteration of the input file.

Note: The program is developed in compliance with CPP 0.0x standards and uses only the iostream library.

