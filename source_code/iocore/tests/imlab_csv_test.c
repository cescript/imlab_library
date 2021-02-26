#include <string.h>
#include "iocore.h"
#include "prcore.h"

int main()
{
    random_seed();

    message(SUCCESS, "starting to test IMLAB csv library");

    // Complex file test
    char *complexFileName = "../../data/sample.csv";
    uint32_t counter = 0;
    // open a csv file with complex rows
    struct csv_t* complex_file_test = csv_open(complexFileName, 0, 1024);

    // print the header
    char header[1024];
    csv_get_next_line(complex_file_test);

    int random_column = random_int(0, csv_get_column_size(complex_file_test) - 1);
    strncpy(header, csv_get_string(complex_file_test, random_column), 1024);

    printf("%s (column %d)\n", header, random_column);
    printf("-----------\n");
    // print the data
    while(csv_get_next_line(complex_file_test))
    {
        printf("[%d]: %s\n", counter++, csv_get_string(complex_file_test, random_column));
    }

    // close the file
    csv_close(&complex_file_test);

    // documentation example
    struct csv_t *table = csv_open("../../data/sample.csv", 0, 1024);
    int cols = csv_get_column_size(table);
    // scan the header line
    csv_get_next_line(table);
    // print the header
    for(int i = 0; i < cols; i++)
    {
        printf("%s \t ", csv_get_string(table, i));
    }
    printf("\n");
    // print the values
    while(csv_get_next_line(table))
    {
        int id = csv_get_long(table, 0);
        char *name = csv_get_string(table, 1);
        double score = csv_get_double(table, 2);
        printf("[%02d] \t [%s] \t [%3.2f]\n", id, name, score);
    }
    csv_close(&table);

    message(SUCCESS, "all operations are done successfully!");

    return 0;
}
