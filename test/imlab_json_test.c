#include "iocore.h"
#include <stdlib.h>

int main() {

    int i;

    message(SUCCESS, "starting to test IMLAB JSON library");

    // create an output directory
    imlab_mkdir("results");

    vector_t *root;
    string_t buffer = string("");
    for(i = 0; i < 1; i++) {
        printf("Loading a JSON file into the memory %d / %d\n", i+1, 1);
        root = json_read("../test/test_data/sample.json");
        printf("JSON file succesfully loaded into the memory\n");
        json_write("results//sample_shrinked.json", root, 0);
        // json_serialize(root, &buffer);
        printf("JSON file succesfully shrinked\n");
        vector_free(&root);
        printf("JSON file succesfully freed\n");
    }
    printf("JSON file succesfully loaded into the memory shrinked and freed\n");

    message(SUCCESS, "all operations are done successfully!");printf("all done ok!\n");

    return 0;
}
