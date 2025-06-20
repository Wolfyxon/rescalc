#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    size_t w;
    size_t h;
} Resolution;

typedef struct {
    char* names;
    Resolution res;
} NamedResolution;

NamedResolution RESOLUTIONS[] = {
    {
        .names = "default fhd 16:9",
        .res = {
            .w = 1920,
            .h = 1080
        }
    }
};

bool strcaseeq(char* a, char* b) {
    size_t len = strlen(a);

    if(strlen(b) != len) {
        return false;
    }

    for(size_t i = 0; i < len; i++) {
        if(a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

Resolution* parseRes(char* str) {
    int str_len = strlen(str);
    
    for(size_t i = 0; i < sizeof(RESOLUTIONS) / sizeof(NamedResolution); i++) {
        NamedResolution* res = &RESOLUTIONS[i];
        size_t names_len = strlen(res->names);

        char buf[names_len + 1];
        strncpy(buf, res->names, names_len + 1);

        char* token = strtok(buf, " ");

        while(token != NULL) {
            if(strcaseeq(token, str)) {
                return &res->res;
            }

            token = strtok(NULL, " "); // NULL tells it to use the previous string. I don't like that 
        }
    }
    
    if(str_len < 3) {
        return NULL;
    }

    int w = -1;
    int h = -1;

    int start = 0;

    for(int i = 0; i < str_len; i++) {
        bool isSplit = str[i] == 'x';

        if(!isSplit && !isdigit(str[i])) {
            return NULL;
        }

        if(isSplit || i == str_len - 1) {
            int dimSize = (str_len + start) - i;
            char buf[dimSize];

            strncpy(buf, str + start, dimSize);

            int parsed = atoi(buf);

            if(w == -1) {
                w = parsed;
            } else if(h == -1) {
                h = parsed;
            } else {
                return NULL;
            }
            
            start = i + 1;
        }
    }

    Resolution* res = malloc(sizeof(Resolution));
    res->w = w;
    res->h = h;
}

Resolution mulRes(Resolution* resolution, float multiplier) {
    return (Resolution) {
        .w = resolution->w * multiplier,
        .h = resolution->h * multiplier
    };
}

Resolution divRes(Resolution* resolution, float divisor) {
    return (Resolution) {
        .w = resolution->w / divisor,
        .h = resolution->h / divisor
    };
}

Resolution* parseResAssert(char* str) {
    Resolution* res = parseRes(str);

    if(res == NULL) {
        fprintf(stderr, "rescalc: '%s' Is not a valid resolution. Example: 1920x1080 \n", str);
        exit(1);
    }

    return res;
}

void printRes(Resolution* resolution) {
    printf("%ix%i\n", resolution->w, resolution->h);
}

bool strisnum(char* str) {
    size_t len = 0;

    bool dot = false;

    for(size_t i = 0; i < len; i++) {
        char c = str[i];

        if(c == '.') {
            if(dot) {
                return false;
            }

            dot = true;
        }

        if(!isdigit(c)) {
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    if(argc == 1) {
        printf("Usage: <resolution> \n");
        exit(1);
    }

    Resolution* resA = parseResAssert(argv[1]);
    // TODO: resolution comparing;
    //Resolution* resB = parseResAssert(argv[2]);

    if(argc < 3) {
        for(float i = 0.25; i <= 2; i += 0.25) {
            Resolution res = mulRes(resA, i);

            if(res.w <= 0 || res.h <= 0) {
                continue;
            }

            // TODO: Symmetrical alignment;
            printf("%g %ix%i \n", i, res.w, res.h);
        }

        return 0;
    }

    if(argc < 4) {
        if(strisnum(argv[2])) {
            float mul = atof(argv[2]);
            
            Resolution res = mulRes(resA, mul);
            printRes(&res);
            
            return 0;
        }
    }

    if(!strisnum(argv[3])) {
        fprintf(stderr, "Usage: rescalc <w>x<h> <* | /> <multiplier / divisor>");
        return 1;
    }

    float modifier = atof(argv[3]);

    if(!strcmp(argv[2], "*") || !strcmp(argv[2], "mul")) {
        Resolution res = mulRes(resA, modifier);
        printRes(&res);
        
        return 0;
    }
    else if(!strcmp(argv[2], "/") || !strcmp(argv[2], "div")) {
        Resolution res = divRes(resA, modifier);
        printRes(&res);
        
        return 0;
    }
}
