#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    size_t w;
    size_t h;
} Resolution;

Resolution* parseRes(char* str) {
    int len = strlen(str);
    
    if(len < 3) {
        return NULL;
    }

    int w = -1;
    int h = -1;

    int start = 0;

    for(int i = 0; i < len; i++) {
        if(str[i] == 'x' || i == len - 1) {
            int dimSize = (len + start) - i;
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

Resolution* parseResAssert(char* str) {
    Resolution* res = parseRes(str);

    if(res == NULL) {
        fprintf(stderr, "rescalc: '%s' Is not a valid resolution. Example: 1920x1080 \n", str);
        exit(1);
    }

    return res;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: <resolution> \n");
        exit(1);
    }

    Resolution* resolution = parseResAssert(argv[1]);

    for(float i = 0.25; i <= 2; i += 0.25) {
        Resolution res = mulRes(resolution, i);

        if(res.w <= 0 || res.h <= 0) {
            continue;
        }


        printf("%g %ix%i \n", i, res.w, res.h);
    }
}
