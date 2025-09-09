#ifndef __DEFS__
#define __DEFS__

#define NCHK(op) do { if ((op) == NULL) { fprintf(stderr, "%s\n", SDL_GetError()); exit(EXIT_FAILURE); } } while(0)
#define CHK(op) do { if ((op) < 0) { fprintf(stderr, "%s\n", SDL_GetError()); exit(EXIT_FAILURE); } } while(0)


#endif // __DEFS__
