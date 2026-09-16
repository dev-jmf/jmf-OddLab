#define _POSIX_C_SOURCE 200809L

#include "web_server.h"

#include "tree_n.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const char page[] =
    "<!doctype html><html lang='pt-BR'><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>TREE(n)</title><style>"
    "*{box-sizing:border-box}body{margin:0;background:#101820;color:#e8f0f2;"
    "font:16px system-ui,sans-serif}main{max-width:1100px;margin:auto;padding:32px}"
    "h1{color:#ffd166;font-size:clamp(28px,5vw,52px);margin:0 0 8px}"
    ".sub{color:#9fb3b8;margin-bottom:28px}.bar{display:flex;gap:10px;flex-wrap:wrap}"
    "input,button{font:inherit;padding:12px 16px;border-radius:6px;border:1px solid #52656b}"
    "input{width:100px;background:#17242b;color:white}button{background:#06d6a0;"
    "border-color:#06d6a0;color:#071c1a;font-weight:700;cursor:pointer}"
    "#status{margin:22px 0;color:#ffd166}.grid{display:grid;grid-template-columns:"
    "repeat(auto-fit,minmax(210px,1fr));gap:14px}.tree{background:#17242b;border:1px solid #30464d;"
    "padding:10px;border-radius:6px}.tree h3{margin:0;color:#06d6a0;font-size:14px}svg{width:100%;height:150px}"
    "line{stroke:#9fb3b8;stroke-width:2}circle{stroke:#101820;stroke-width:2}" 
    "</style></head><body><main><h1>TREE(n)</h1>"
    "<div class='sub'>Simulador exploratorio em C, executando no Codespace.</div>"
    "<div class='bar'><label for='n'>n:</label><input id='n' type='number' min='1' max='12' value='3'>"
    "<button id='run'>Executar</button></div><div id='status'>Pronto.</div><section id='trees' class='grid'></section>"
    "</main><script>"
    "const colors=['#06d6a0','#ffd166','#ef476f','#118ab2','#f78c6b','#c77dff'];"
    "const out=document.querySelector('#trees'),status=document.querySelector('#status');"
    "function nodeSvg(n,x,y,dx){let s='';if(n.children.length){let step=dx/n.children.length;"
    "n.children.forEach((c,i)=>{let cx=x-dx/2+step*(i+.5);s+=`<line x1='${x}' y1='${y}' x2='${cx}' y2='${y+42}'/>`+nodeSvg(c,cx,y+42,step)})}"
    "return s+`<circle cx='${x}' cy='${y}' r='8' fill='${colors[n.color%colors.length]}'/>`}" 
    "async function run(){let n=document.querySelector('#n').value;status.textContent='Calculando...';"
    "try{let r=await fetch('/run?n='+encodeURIComponent(n));let d=await r.json();if(!r.ok)throw Error(d.error);"
    "status.textContent=`n=${d.n} | arvores aceitas: ${d.count} | candidatos verificados: ${d.checked}`;"
    "out.innerHTML=d.trees.map((t,i)=>`<article class='tree'><h3>Arvore ${i+1} - ${t.size} nos</h3><svg viewBox='0 0 200 150'>${nodeSvg(t,100,18,160)}</svg></article>`).join('')"
    "}catch(e){status.textContent='Erro: '+e.message;out.innerHTML=''}}document.querySelector('#run').onclick=run;run();"
    "</script></body></html>";

static void send_text(int client, const char *type, const char *body)
{
    dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n%s",
            type, strlen(body), body);
}

static void send_error(int client, const char *message)
{
    dprintf(client, "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n"
                    "Content-Length: %zu\r\nConnection: close\r\n\r\n{\"error\":\"%s\"}",
            strlen(message) + 12, message);
}

static void write_json_tree(int client, const TreeNode *tree)
{
    size_t i;

    dprintf(client, "{\"color\":%u,\"size\":%zu,\"children\":[", tree->color,
            tree_size(tree));
    for (i = 0; i < tree->child_count; ++i) {
        if (i > 0) dprintf(client, ",");
        write_json_tree(client, tree->children[i]);
    }
    dprintf(client, "]}");
}

static int query_n(const char *request)
{
    const char *value = strstr(request, "GET /run?n=");
    char *end;
    long n;

    if (value == NULL) return 0;
    n = strtol(value + 11, &end, 10);
    if (end == value + 11 || n < 1 || n > 12) return 0;
    return (int)n;
}

static void handle_client(int client)
{
    char request[2048] = {0};
    ssize_t received = recv(client, request, sizeof(request) - 1, 0);

    if (received <= 0) return;
    if (strncmp(request, "GET /run?", 9) == 0) {
        TreeSimulation simulation;
        int n = query_n(request);
        size_t i;
        if (n == 0) {
            send_error(client, "n deve ser um inteiro entre 1 e 12");
            return;
        }
        simulation_init(&simulation);
        if (!simulation_run(&simulation, n)) {
            simulation_clear(&simulation);
            send_error(client, "nao foi possivel executar a simulacao");
            return;
        }
        dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
                        "Connection: close\r\n\r\n{\"n\":%d,\"count\":%zu,\"checked\":%zu,\"trees\":[",
                n, simulation.count, simulation.candidates_checked);
        for (i = 0; i < simulation.count; ++i) {
            if (i > 0) dprintf(client, ",");
            write_json_tree(client, simulation.trees[i]);
        }
        dprintf(client, "]}");
        simulation_clear(&simulation);
    } else if (strncmp(request, "GET / ", 6) == 0) {
        send_text(client, "text/html; charset=utf-8", page);
    } else {
        send_error(client, "rota nao encontrada");
    }
}

int web_server_run(int port)
{
    int server;
    int option = 1;
    struct sockaddr_in address = {0};

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) return 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((unsigned short)port);
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0 || listen(server, 8) < 0) {
        fprintf(stderr, "Nao foi possivel iniciar o servidor HTTP na porta %d: %s\n", port, strerror(errno));
        close(server);
        return 1;
    }
    printf("Interface web disponivel em http://localhost:%d\n", port);
    printf("Mantenha este processo aberto e abra essa URL no navegador.\n");
    fflush(stdout);
    for (;;) {
        int client = accept(server, NULL, NULL);
        if (client >= 0) {
            handle_client(client);
            close(client);
        }
    }
}