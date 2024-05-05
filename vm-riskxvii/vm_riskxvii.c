//520476042,TONGYU,HU,TOHU8281@UNI.SYDNEY.EDU.AU,-,Wed,10:00,11,COMP2017-S1C-ND-CC/ASSESSMENT/39,2023/04/19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LENGTH 4
#define MEMORY_SIZE 4096
#define REGISTER_SIZE 32

int main(int argc, char **argv) {
    //Initialise variables 
    int num_instructions;
    uint32_t* instruction;
    uint32_t PC = 0;

    //Initialise virtual memory locations
    int int_console_halt = 2060;
    int int_console_read_signed = 2070;
    int int_console_read_char = 2066;
    int int_console_write = 2048;
    int int_console_write_signed = 2052;
    int int_console_write_unsigned = 2056;
    int twelve_signed_conversion = 2048;
    int twenty_signed_conversion = 524288;

    //Set up instruction, buffer, memory and registers
    char binary_instruction[256][33];
    uint8_t buffer[MAX_LENGTH];
    int registers[REGISTER_SIZE];
    int memory[MEMORY_SIZE];

    //Initialise registers with 0
    for(int i = 0; i < 32; i++){
        registers[i] = 0;
    }

    //check if enough arguments
    if(argc < 1){
        printf("Not enough arguments!");
        exit(1);
    }

    //Open binary file
    FILE* binary_file = fopen(argv[1], "rb");

    //Check if file exists
    if(binary_file == NULL){
        printf("Error reading file");
        exit(1);
    }

    //Get number of instructions
    while(fread(buffer, sizeof(uint8_t), MAX_LENGTH, binary_file) > 0) {
        int i = 0;
        while(i < MAX_LENGTH) {
            instruction = (uint32_t*)&buffer[i];
            // Check if the instruction is valid
            if(*instruction != 0) {
                num_instructions++;
            }
            i += 4; 
        }
    }

    //Reset file pointer
    rewind(binary_file);

    //Allocate memory for instructionsa
    instruction = malloc(num_instructions * sizeof(uint32_t));
    if(instruction == NULL){
        printf("Error allocating memory");
        exit(0);
    }

    //Setup memory location variables
    int counter = 0;
    int memory_location = 80;

    for(int i = 0; i < num_instructions; i++){
        //Read instructions from binary file 
        fread(buffer, sizeof(uint8_t), MAX_LENGTH, binary_file);

        //Combine 4 bytes of the instruction in the buffer to 32 bit integer
        instruction[i] = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
        
        //Check validity of instruction obtained
        if(sizeof(instruction[i]) != 4){
            printf("Instruction requirements not met");
            exit(1);
        }

        //Convert to binary string
        for (int j = 0; j <= 31; j++) {

            //Check if jth bit of instruction[i] is set to 1 (shifting integer 1 to the left by j bits)
            if(instruction[i] & (1 << j)){
                binary_instruction[i * 4][31 - j] = '1';
            }else{ 
                binary_instruction[i * 4][31 - j] = '0';
            }
        }
        binary_instruction[i * 4][32] = '\0';

        //Retrieve memory data and store in memory
        if(i >= 20){
            memory[memory_location + counter] = buffer[0];
            counter++;
            memory[memory_location + counter] = buffer[1];
            counter++;
            memory[memory_location + counter] = buffer[2];
            counter++;
            memory[memory_location + counter] = buffer[3];
            counter++;
        }
    }

    //Free memory allocated for instruction
    free(instruction);

    //Iterate through instructions
    while(PC < num_instructions * 4){

        //Get Opcode
        char op_code[10];
        for(int y = 25; y < 32; y++){
            op_code[y - 25] = binary_instruction[PC][y];
        }
        op_code[7] = '\0';
        
        //Type I and op_code = 0010011
        if(strcmp(op_code, "0010011") == 0){
            //Grab elements from binary string
            char rd[6];
            for(int y = 20; y < 25; y++){
                rd[y - 20] = binary_instruction[PC][y];
                rd[5] = '\0';
            }
            char funct3[4];
            for(int y = 17; y < 20; y++){
                funct3[y - 17] = binary_instruction[PC][y];
                funct3[3] = '\0';
            }
            char rs1[6];
            for(int y = 12; y < 17; y++){
                rs1[y - 12] = binary_instruction[PC][y];
                rs1[5] = '\0';
            }
            char imm[13];
            for(int y = 0; y < 12; y++){
                imm[y] = binary_instruction[PC][y];
                imm[12] = '\0';
            }
            
            //addi function
            if(strcmp(funct3, "000") == 0){
                //Convert to int from binary string
                int rd_int = strtol(rd, NULL, 2);
                int rs1_int = strtol(rs1, NULL, 2);

                //Check for zero register
                if(rd_int == 0){
                    PC += 4;
                    continue;
                }
                else{
                    //Perform sign extension conversion if required
                    char to_imm[2];
                    to_imm[0] = imm[0];
                    to_imm[1] = '\0';
                    if(strcmp(to_imm, "1") == 0){
                        imm[0] = '0';
                        int imm_int = strtol(imm, NULL, 2);
                        registers[rd_int] = registers[rs1_int] - (twelve_signed_conversion - imm_int);
                    }
                    else{
                        int imm_int = strtol(imm, NULL, 2);
                        registers[rd_int] = registers[rs1_int] + imm_int;
                    }
                    PC += 4;
                }
            }
        }

        //Type I and op_code = 1100111
        else if(strcmp(op_code, "1100111") == 0){
            //Grab elements from binary string
            char rd[6];
            for(int y = 20; y < 25; y++){
                rd[y - 20] = binary_instruction[PC][y];
                rd[5] = '\0';
            }
            char funct3[4];
            for(int y = 17; y < 20; y++){
                funct3[y - 17] = binary_instruction[PC][y];
                funct3[3] = '\0';
            }
            char rs1[6];
            for(int y = 12; y < 17; y++){
                rs1[y - 12] = binary_instruction[PC][y];
                rs1[5] = '\0';
            }
            char imm[13];
            for(int y = 0; y < 12; y++){
                imm[y] = binary_instruction[PC][y];
                imm[12] = '\0';
            }
            
            //jalr function
            if(strcmp(funct3, "000") == 0){
                //Convert to int from binary string
                int rd_int = strtol(rd, NULL, 2);
                int rs1_int = strtol(rs1, NULL, 2);
                
                //Perform sign extension conversion if required
                char to_imm[2];
                to_imm[0] = imm[0];
                to_imm[1] = '\0';
                char temp_imm[13];
                int imm_int;
                if(strcmp(to_imm, "1") == 0){
                    temp_imm[0] = '0';
                    for(int i = 1; i < 13; i++){
                        temp_imm[i] = imm[i];
                    }
                    imm_int = strtol(temp_imm, NULL, 2) - twelve_signed_conversion;
                }
                else{
                    imm_int = strtol(imm, NULL, 2);
                }

                //Perform operation and check for zero register
                if(rd_int == 0){
                    registers[rd_int] = 0;
                }
                else{
                    registers[rd_int] = PC + 4;
                }
                PC = registers[rs1_int] + imm_int;
            }
        }

        //Type U and op_code = 0110111 
        //lui function
        else if(strcmp(op_code, "0110111") == 0){
            //Grab elements from binary string
            char rd[6];
            for(int y = 20; y < 25; y++){
                rd[y - 20] = binary_instruction[PC][y];
                rd[5] = '\0';
            }
            char imm[21];
            for(int y = 0; y < 20; y++){
                imm[y] = binary_instruction[PC][y];
                imm[20] = '\0';
            }

            //Convert to int from binary string
            int rd_int = strtol(rd, NULL, 2);
            int imm_int = strtol(imm, NULL, 2);
            
            //Perform operation
            registers[rd_int] = imm_int << 12;
            PC += 4;
        }

        //Type UJ and op_code = 1101111
        //jal function
        else if(strcmp(op_code, "1101111") == 0){
            //Grab elements from binary string
            char rd[6];
            for(int y = 20; y < 25; y++){
                rd[y - 20] = binary_instruction[PC][y];
                rd[5] = '\0';
            }
            char imm[21];
            imm[0] = binary_instruction[PC][0];
            for(int y = 11; y < 20; y++){
                imm[y - 10] = binary_instruction[PC][y];
            }
            imm[10] = binary_instruction[PC][10];
            for(int y = 1; y < 11; y++){
                imm[y + 9] = binary_instruction[PC][y];
            }
            imm[20] = '\0'; 
            
            //Convert to int from binary string
            int rd_int = strtol(rd, NULL, 2);
            char to_imm[2];
            to_imm[0] = imm[0];
            to_imm[1] = '\0';
            char temp_imm[21];
            int imm_int;

            //Perform sign extension conversion if required
            if(strcmp(to_imm, "1") == 0){
                temp_imm[0] = '0';
                for(int i = 1; i < 21; i++){
                    temp_imm[i] = imm[i];
                }
                imm_int = strtol(temp_imm, NULL, 2) - twenty_signed_conversion;
            }
            else{
                imm_int = strtol(imm, NULL, 2);
            }

            //Perform operation and check for zero register
            if(rd_int == 0){
                registers[rd_int] = 0;
            }
            else{
                registers[rd_int] = PC + 4;
            }
            PC += imm_int << 1;
        }

        //Type S and op_code = 0100011
        else if(strcmp(op_code, "0100011") == 0){
            //Grab elements from binary string       
            char funct3[4];
            for(int y = 17; y < 20; y++){
                funct3[y - 17] = binary_instruction[PC][y];
            }
            funct3[3] = '\0';

            char rs1[6];
            for(int y = 12; y < 17; y++){
                rs1[y - 12] = binary_instruction[PC][y];
            }
            rs1[5] = '\0';

            char rs2[6];
            for(int y = 7; y < 12; y++){
                rs2[y - 7] = binary_instruction[PC][y];
            }
            rs2[5] = '\0';

            char imm[13];
            for(int y = 0; y < 7; y++){
                imm[y] = binary_instruction[PC][y];
            }
            for(int y = 20; y < 25; y++){
                imm[y - 13] = binary_instruction[PC][y];
            }
            imm[12] = '\0';

            //sb function
            if(strcmp(funct3, "000") == 0){
                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rs2_int = strtol(rs2, NULL, 2);

                //Perform sign extension conversion if required
                char to_imm[2];
                to_imm[0] = imm[0];
                to_imm[1] = '\0';
                char temp_imm[13];
                int imm_int;

                if(strcmp(to_imm, "1") == 0){
                    temp_imm[0] = '0';
                    for(int i = 1; i < 13; i++){
                        temp_imm[i] = imm[i];
                    }
                    imm_int = strtol(temp_imm, NULL, 2) - twelve_signed_conversion;
                }
                else{
                    imm_int = strtol(imm, NULL, 2);
                }
                
                //Perform operation (Store in memory)
                int address_found = registers[rs1_int] + imm_int;
                uint8_t element = (uint8_t)registers[rs2_int];
                memory[address_found] = element;

                //Check if address is virtual address
                if(registers[rs1_int] + imm_int == int_console_write){
                    printf("%c", registers[rs2_int]);
                }
                else if(registers[rs1_int] + imm_int == int_console_halt){
                    printf("CPU Halt Requested\n");
                    fclose(binary_file);
                    return 0;
                }
                PC += 4;
            }

            //sw function
            else if(strcmp(funct3, "010") == 0){
                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rs2_int = strtol(rs2, NULL, 2);

                //Perform sign extension conversion if required
                char to_imm[2];
                to_imm[0] = imm[0];
                to_imm[1] = '\0';
                char temp_imm[13];
                int imm_int;

                if(strcmp(to_imm, "1") == 0){
                    temp_imm[0] = '0';
                    for(int i = 1; i < 13; i++){
                        temp_imm[i] = imm[i];
                    }
                    imm_int = strtol(temp_imm, NULL, 2) - twelve_signed_conversion;
                }
                else{
                    imm_int = strtol(imm, NULL, 2);
                }

                //Perform operation (Store in memory)
                int address_found = registers[rs1_int] + imm_int;
                uint32_t element = (uint32_t)registers[rs2_int];
                memory[address_found] = element;

                //Check if operation is a virtual address and perform accordingly
                if(registers[rs1_int] + imm_int == int_console_write){
                    printf("%c", registers[rs2_int]);
                }
                else if(registers[rs1_int] + imm_int == int_console_write_signed){
                    printf("%d", registers[rs2_int]);

                }
                else if(registers[rs1_int] + imm_int == int_console_write_unsigned){
                    if(registers[rs2_int] == -1){
                        printf("1");
                    }
                    else{
                        printf("%x", registers[rs2_int]);
                    }
                }
                else if(registers[rs1_int] + imm_int == int_console_halt){
                    printf("CPU Halt Requested\n");
                    fclose(binary_file);
                    return 0;
                }
                PC += 4;
            }
        }

        //Type R and op_code = 0110011
        else if(strcmp(op_code, "0110011") == 0){
            //Grab elements from binary string
            char rd[6];
            for(int y = 20; y < 25; y++){
                rd[y - 20] = binary_instruction[PC][y];
                rd[5] = '\0';
            }
            char funct3[4];
            for(int y = 17; y < 20; y++){
                funct3[y - 17] = binary_instruction[PC][y];
                funct3[3] = '\0';
            }
            char rs1[6];
            for(int y = 12; y < 17; y++){
                rs1[y - 12] = binary_instruction[PC][y];
                rs1[5] = '\0';
            }
            char rs2[6];
            for(int y = 7; y < 12; y++){
                rs2[y - 7] = binary_instruction[PC][y];
                rs2[5] = '\0';
            }
            char funct7[8];
            for(int y = 0; y < 7; y++){
                funct7[y] = binary_instruction[PC][y];
                funct7[7] = '\0';
            }

            //add function
            if(strcmp(funct3, "000") == 0 && strcmp(funct7, "0000000") == 0){

                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rs2_int = strtol(rs2, NULL, 2);
                int rd_int = strtol(rd, NULL, 2);

                //Perform operation
                registers[rd_int] = registers[rs1_int] + registers[rs2_int];
                PC += 4;
            }

            //sub function
            else if(strcmp(funct3, "000") == 0 && strcmp(funct7, "0100000") == 0){

                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rs2_int = strtol(rs2, NULL, 2);
                int rd_int = strtol(rd, NULL, 2);

                //Perform operation
                registers[rd_int] = registers[rs1_int] - registers[rs2_int];
                PC += 4;
            }

            //sll function
            else if(strcmp(funct3, "001") == 0 && strcmp(funct7, "0000000") == 0){

                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rs2_int = strtol(rs2, NULL, 2);
                int rd_int = strtol(rd, NULL, 2);

                //Perform operation
                registers[rd_int] = registers[rs1_int] << registers[rs2_int];
                PC += 4;
            }

            //srl function
            else if(strcmp(funct3, "101") == 0 && strcmp(funct7, "0000000") == 0){

                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rs2_int = strtol(rs2, NULL, 2);
                int rd_int = strtol(rd, NULL, 2);

                //Perform operation
                registers[rd_int] = registers[rs1_int] >> registers[rs2_int];
                PC += 4;
            }
            
        }
        //Type I and op_code = 0000011
        else if(strcmp("0000011", op_code) == 0){
            //Grab elements from binary string
            char rd[6];
            for(int y = 20; y < 25; y++){
                rd[y - 20] = binary_instruction[PC][y];
                rd[5] = '\0';
            }
            char funct3[4];
            for(int y = 17; y < 20; y++){
                funct3[y - 17] = binary_instruction[PC][y];
                funct3[3] = '\0';
            }
            char rs1[6];
            for(int y = 12; y < 17; y++){
                rs1[y - 12] = binary_instruction[PC][y];
                rs1[5] = '\0';
            }
            char imm[13];
            for(int y = 0; y < 12; y++){
                imm[y] = binary_instruction[PC][y];
                imm[12] = '\0';
            }

            //lw function
            if(strcmp(funct3, "010") == 0){

                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rd_int = strtol(rd, NULL, 2);

                //Perform sign extension conversion if required
                char to_imm[2];
                to_imm[0] = imm[0];
                to_imm[1] = '\0';
                char temp_imm[13];
                int imm_int;

                if(strcmp(to_imm, "1") == 0){
                    temp_imm[0] = '0';
                    for(int i = 1; i < 13; i++){
                        temp_imm[i] = imm[i];
                    }
                    imm_int = strtol(temp_imm, NULL, 2) - twelve_signed_conversion;
                }
                else{
                    imm_int = strtol(imm, NULL, 2);
                }

                //Perform operation and check for console read (virtual memory)
                if (registers[rs1_int] + imm_int == int_console_read_signed){
                    scanf("%d", &registers[rd_int]);
                }
                else{
                    //Perform operation and checking for zero register
                    if (rd_int == 0){
                        registers[rd_int] = 0;
                    }
                    else{
                        registers[rd_int] = memory[registers[rs1_int] + imm_int];
                    }
                }
                PC += 4;
            }

            //lbu function
            else if(strcmp(funct3, "100") == 0){
                //Convert to int from binary string
                int rs1_int = strtol(rs1, NULL, 2);
                int rd_int = strtol(rd, NULL, 2);

                //Perform sign extension conversion if required
                char to_imm[2];
                to_imm[0] = imm[0];
                to_imm[1] = '\0';
                char temp_imm[13];
                int imm_int;

                if(strcmp(to_imm, "1") == 0){
                    temp_imm[0] = '0';
                    for(int i = 1; i < 13; i++){
                        temp_imm[i] = imm[i];
                    }
                    imm_int = strtol(temp_imm, NULL, 2) - twelve_signed_conversion;
                }
                else{
                    imm_int = strtol(imm, NULL, 2);
                }

                //Perform operation and check for console read (virtual memory)
                if (registers[rs1_int] + imm_int == int_console_read_signed){
                    scanf("%d", &registers[rd_int]);
                }
                else if(registers[rs1_int] + imm_int == int_console_read_char){
                    scanf("%d", &registers[rd_int]);
                }
                else{
                    //Perform operation and checking for zero register
                    if(rd_int == 0){
                        registers[rd_int] = 0;
                    }
                    else{
                        registers[rd_int] = memory[registers[rs1_int] + imm_int];
                    }
                }
                PC += 4;
            }

        }

        //Type SB and op_code = 1100011
        else if(strcmp(op_code, "1100011") == 0){
            //Grab elements from binary string
            char funct3[4];
            for(int y = 17; y < 20; y++){
                funct3[y - 17] = binary_instruction[PC][y];
                funct3[3] = '\0';
            }
            char rs1[6];
            for(int y = 12; y < 17; y++){
                rs1[y - 12] = binary_instruction[PC][y];
                rs1[5] = '\0';
            }
            char rs2[6];
            for(int y = 7; y < 12; y++){
                rs2[y - 7] = binary_instruction[PC][y];
                rs2[5] = '\0';
            }
            char imm[13];
            imm[0] = binary_instruction[PC][0];
            imm[1] = binary_instruction[PC][24];
            for(int y = 1; y < 7; y++){
                imm[y + 1] = binary_instruction[PC][y];
            }
            for(int y = 1; y < 5; y++){
                imm[y + 7] = binary_instruction[PC][y + 19];
            }
            imm[12] = '\0';

            //Convert to int from binary string
            int rs1_int = strtol(rs1, NULL, 2);
            int rs2_int = strtol(rs2, NULL, 2);

            //Perform sign extension conversion if required
            char to_imm[2];
            to_imm[0] = imm[0];
            to_imm[1] = '\0';
            char temp_imm[13];
            int imm_int;

            if(strcmp(to_imm, "1") == 0){
                temp_imm[0] = '0';
                for(int i = 1; i < 13; i++){
                    temp_imm[i] = imm[i];
                }
                imm_int = strtol(temp_imm, NULL, 2) - twelve_signed_conversion;
            }
            else{
                imm_int = strtol(imm, NULL, 2);
            }
    
            //beq function
            if(strcmp(funct3, "000") == 0){
                //Perform operation and reset zero register
                registers[0] = 0;
                if(registers[rs1_int] == registers[rs2_int]){
                    PC += imm_int << 1;
                }
                else{
                    PC += 4;
                }
            }
            //bne function
            else if(strcmp(funct3, "001") == 0){
                //Perform operation and reset zero register
                registers[0] = 0;
                if(registers[rs1_int] != registers[rs2_int]){
                    PC += imm_int << 1;
                }
                else{
                    PC += 4;
                }
            }
            registers[0] = 0;
        }
    }

    //Close file
    fclose(binary_file);
    return 1;
}