#include <stdlib.h>
#include "server.h"
#include "atomicvar.h"
#include <stdlib.h>
#include <string.h>

client *g_client;

// TODO: add handling of the initialization process errors
void init() {
    initRedis(0, NULL);

    // create a main client
    g_client = createClient(-1);
}

int g_isInited = 0;

void* allocate(size_t size) {
    // allocate one more byte for adding the \n symbol that indicated the end of request
    return zmalloc(size + 1);
}

void deallocate(void *ptr, int size) {
    zfree(ptr);
}

void store(char *ptr, unsigned char byte) {
    *ptr = byte;
}

unsigned char load(const unsigned char *ptr) {
    return *ptr;
}

// Cleans client output buffers to - client is blocked and
// doesn't receive requests until output buffer isn't empty.
void clean_client_buffer(client *c) {
    freeClientArgv(c);
    zfree(c->argv);
    freeClientMultiState(c);
    sdsfree(c->peerid);

    c->reply_bytes = 0;
    c->bufpos = 0;
    c->argc = 0;
    c->argv = NULL;
    c->peerid = NULL;

    // clear the query buffer state
    sdsclear(c->querybuf);
    c->qb_pos = 0;

    // clear the client state
    c->flags = 0;

    // set reqtype to unknown to force Redis determine it on every request
    c->reqtype = 0;
}

char *write_response(client *c, size_t *response_size) {
    char *response = allocate(c->bufpos + c->reply_bytes + 4);
    *response_size = 0;

    memcpy(response + 4, c->buf, c->bufpos);
    *response_size += c->bufpos;

    while(listLength(c->reply)) {
        clientReplyBlock *o = listNodeValue(listFirst(c->reply));
        size_t objlen = o->used;

        if (objlen == 0) {
            listDelNode(c->reply, listFirst(c->reply));
            continue;
        }

        memcpy(response + 4 + *response_size, o->buf, objlen);

        c->reply_bytes -= o->size;
        *response_size += objlen;
        listDelNode(c->reply, listFirst(c->reply));
    }

    for(int i = 0; i < 4; ++i) {
        response[i] = (*response_size >> 8*i) & 0xFF;
    }

    return response;
}

const char *invoke(char *request, int request_size) {
    serverLog(LL_DEBUG, "invoke started\n");
    if(g_isInited == 0) {
        init();
        serverLog(LL_DEBUG, "\nserver has been inited\n");
        g_isInited = 1;
    }

    afterSleep();
    serverLog(LL_DEBUG, "afterSleep\n");

    if(request_size > 0 && request[request_size - 1] != '\n') {
        // WasmVM always uses allocate function to inject requests to Wasm memory. And it always allocates one more byte
        // especially for '\n' symbol
        request[request_size] = '\n';
        readQueryFromClient(g_client, 0, request, request_size + 1);
    } else {
        readQueryFromClient(g_client, 0, request, request_size);
    }
    deallocate(request, 0);
    serverLog(LL_DEBUG, "readQueryFromClient\n");

    const size_t reply_bytes_before = g_client->reply_bytes;
    size_t response_size = 0;
    const char *response = write_response(g_client, &response_size);
    serverLog(LL_DEBUG, "write_response, bufpos = %d, reply_bytes before = %d, reply_bytes after = %d, response_size = %d\n",
            g_client->bufpos,
            reply_bytes_before,
            g_client->reply_bytes,
            response_size
            );
    clean_client_buffer(g_client);

    serverCron();
    serverLog(LL_DEBUG, "serverCron\n");

    beforeSleep();
    serverLog(LL_DEBUG, "beforeSleep\n");

    return response;
}

int main(){
  invoke("sadfasdfsdafsadfa",100);
}