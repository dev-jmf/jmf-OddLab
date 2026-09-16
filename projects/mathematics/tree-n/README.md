# TREE(n) em C

Pequeno visualizador experimental para estudar o crescimento de sequencias de
arvores rotuladas. A janela recebe um valor de `n`, executa uma busca gulosa e
desenha as primeiras arvores aceitas.

## Compilar e executar

Em Linux com X11 e os headers de desenvolvimento instalados:

```sh
make
./tree-n
```

Em um Codespace sem `DISPLAY`, o programa inicia automaticamente uma interface
web em `http://localhost:8080`. Deixe o processo aberto e abra essa URL no
navegador; o VS Code pode pedir para encaminhar a porta 8080.

Fora do Codespace, o programa usa a janela nativa X11. Portanto, em um Linux
local com uma sessao grafica, basta executar normalmente. Se `DISPLAY` estiver
vazio no computador local, sera necessario iniciar o programa em uma sessao X11.

Digite um inteiro de `1` a `12` no campo e pressione `Enter` ou clique em
`Executar`. O limite baixo e intencional: a enumeração cresce rapidamente.

## Modelo usado

Este programa e uma ferramenta de exploracao, nao um calculador de `TREE(3)`.
Cada arvore e enraizada, tem um rotulo de cor, e a simulacao tenta adicionar
uma arvore por cor em cada rodada. Uma candidata e rejeitada quando alguma
arvore anterior a contem como subarvore enraizada e com os mesmos rotulos.

Os modulos estao separados para permitir evolucao:

- `tree.c/.h`: estrutura e ciclo de vida das arvores;
- `embedding.c/.h`: teste de embedding;
- `tree_n.c/.h`: estado e geracao da simulacao;
- `main.c`: interface grafica X11.

Uma proxima etapa importante seria implementar geracao sistematica de todas as
arvores validas dentro do limite de nos, em vez da atual amostra gulosa.