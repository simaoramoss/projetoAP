# BikeRent — Gestão de Aluguer de Bicicletas

Projeto Prático de Algoritmos e Programação 2026/27.

## Compilar e executar

```bash
make
./bikerent
```

Sem `make`: `gcc -Wall -Wextra -o bikerent *.c`

Os ficheiros `Bicicletas_Instalacao.txt` e `Tarifario_instalacao.txt` têm de estar
na pasta de onde o programa é executado. Na primeira utilização o programa instala
a partir deles e cria `bicicletas.dat`, `alugueres.dat` e `tarifario.dat`; nas
seguintes carrega esses binários. Para repetir a instalação, apagar os `.dat`.

## Antes de entregar

Preencher o nome e o grupo em `identificacao_grupo.txt`. `NN` (97) está em `constantes.h`
e os três parâmetros do grupo são calculados a partir dele.

## Organização do código

| Ficheiro | Conteúdo |
|---|---|
| `constantes.h` | Constantes, incluindo a personalização por grupo |
| `estruturas.h` | `Bicicleta`, `Aluguer`, `Tarifa`, `Loja` |
| `utils.c` | Leitura validada, datas, paginação, gravação de listagens |
| `bicicletas.c` | Frota: registar, colocar em serviço, eliminar, manutenção |
| `alugueres.c` | Levantamento, devolução, cancelamento, regra de preços |
| `ficheiros.c` | Instalação pelos `.txt` e persistência em binário |
| `listagens.c` | Listagens, estatísticas e tarifário |
| `main.c` | Menus |
