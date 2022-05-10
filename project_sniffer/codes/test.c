#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void update_timing_config_file(uint32_t address, uint32_t data, uint32_t data_width_bits,
                               uint32_t current_transaction_count, uint32_t current_transfer_count, uint32_t current_data_word_count,
                               uint32_t total_transaction_count, uint32_t total_transfer_count, uint32_t total_data_word_count)
{
    FILE *fptr1, *fptr2;
    char *line = NULL;
    char *destLine = NULL; 
    size_t len = 0;
    ssize_t read;

	switch(data_width_bits)
	{
		case 128: 
		{
            // Open one file for reading
            fptr1 = fopen("128_bit.txt", "r");
            if (fptr1 == NULL)
            {
                printf("Cannot open file 128_bit.txt\n");
                return;
            }
  
            // Open another file for writing
            fptr2 = fopen("timing_config.txt", "w");
            if (fptr2 == NULL)
            {
                printf("Cannot open file timing_config.txt \n");
                return;
            }
  
            // Read contents from file line by line   
			while (getline(&line, &len, fptr1) != -1) 
			{
                char str[500];
                if (strstr(line, "Transaction Count") != NULL) 
                {
                    snprintf(str, 500, "  {name: 'Transaction Count (%d/%d Transactions)', wave: '010......10.......'},\n", current_transaction_count, total_transaction_count);
                    fputs(str, fptr2);
                }
                else if (strstr(line, "Transfer Count") != NULL) 
                {
                    snprintf(str, 500, "  {name: 'Transfer Count (%d/%d Bursts)', wave: '010..10..10..10...'},\n", current_transfer_count, total_transfer_count);
                    fputs(str, fptr2);
                }
                else if (strstr(line, "Data Word") != NULL) 
                {
                    if(current_transfer_count == 1 && current_data_word_count == 1)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 1 && current_data_word_count == 2)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 1 && current_data_word_count == 3)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 1 && current_data_word_count == 4)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 2 && current_data_word_count == 1)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 2 && current_data_word_count == 2)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 2 && current_data_word_count == 3)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    else if(current_transfer_count == 2 && current_data_word_count == 4)
                    {
                        snprintf(str, 500, "  {name: 'Data Word (%d/%d Words)', wave: 'x3456345634563456x', data: ['0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0x%.8X', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC', '0xCCCCCCCC']}\n", current_data_word_count, total_data_word_count, data);
                    }
                    fputs(str, fptr2);
                }
                else if (strstr(line, "Address Probed") != NULL) 
                {
                    snprintf(str, 500, "   text:'Address Probed - 0x%.8X',\n", address);
                    fputs(str, fptr2);
                }
                else
                {
                    fputs(line, fptr2);
                }
			}

            fclose(fptr1);
            fclose(fptr2);	
            if (line)
                free(line);		
		}
		break;
	}
}

int main()
{
    update_timing_config_file(0xFFFC0022, 0xDEADBEEF, 128, 1, 2, 2, 2, 2, 4);
    return 0;
}
