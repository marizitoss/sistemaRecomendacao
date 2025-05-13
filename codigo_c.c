#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 3

typedef struct Livro {
    int id;
    char titulo[100];
    char autor[100];
    char isbn[20];
    char genero[100];
    float preco;
    int quantidadeEstoque;
    int anoPublicacao;
    char editora[100];
    char sinopse[256];
} Livro;

typedef struct BTreeNode {
    char chave[2 * MAX][200];
    Livro *livros[2 * MAX];
    struct BTreeNode *filhos[2 * MAX + 1];
    int n;
    int folha;
} BTreeNode;

BTreeNode *criarNo(int folha) {
    BTreeNode *novo = (BTreeNode *)malloc(sizeof(BTreeNode));
    novo->folha = folha;
    novo->n = 0;
    for (int i = 0; i < 2 * MAX + 1; i++)
        novo->filhos[i] = NULL;
    return novo;
}

void inserirNaoCheio(BTreeNode *x, char *k, Livro *livro);
void dividirFilho(BTreeNode *x, int i, BTreeNode *y);
void inserir(BTreeNode **raiz, char *k, Livro *livro) {
    BTreeNode *r = *raiz;
    if (r->n == 2 * MAX) {
        BTreeNode *s = criarNo(0);
        s->filhos[0] = r;
        dividirFilho(s, 0, r);
        inserirNaoCheio(s, k, livro);
        *raiz = s;
    } else {
        inserirNaoCheio(r, k, livro);
    }
}

void dividirFilho(BTreeNode *x, int i, BTreeNode *y) {
    BTreeNode *z = criarNo(y->folha);
    z->n = MAX;
    for (int j = 0; j < MAX; j++) {
        strcpy(z->chave[j], y->chave[j + MAX + 1]);
        z->livros[j] = y->livros[j + MAX + 1];
    }
    if (!y->folha) {
        for (int j = 0; j <= MAX; j++)
            z->filhos[j] = y->filhos[j + MAX + 1];
    }
    y->n = MAX;
    for (int j = x->n; j >= i + 1; j--)
        x->filhos[j + 1] = x->filhos[j];
    x->filhos[i + 1] = z;
    for (int j = x->n - 1; j >= i; j--) {
        strcpy(x->chave[j + 1], x->chave[j]);
        x->livros[j + 1] = x->livros[j];
    }
    strcpy(x->chave[i], y->chave[MAX]);
    x->livros[i] = y->livros[MAX];
    x->n++;
}

void inserirNaoCheio(BTreeNode *x, char *k, Livro *livro) {
    int i = x->n - 1;
    if (x->folha) {
        while (i >= 0 && strcmp(k, x->chave[i]) < 0) {
            strcpy(x->chave[i + 1], x->chave[i]);
            x->livros[i + 1] = x->livros[i];
            i--;
        }
        strcpy(x->chave[i + 1], k);
        x->livros[i + 1] = livro;
        x->n++;
    } else {
        while (i >= 0 && strcmp(k, x->chave[i]) < 0)
            i--;
        i++;
        if (x->filhos[i]->n == 2 * MAX) {
            dividirFilho(x, i, x->filhos[i]);
            if (strcmp(k, x->chave[i]) > 0)
                i++;
        }
        inserirNaoCheio(x->filhos[i], k, livro);
    }
}

void imprimir(BTreeNode *raiz, int nivel) {
    if (raiz) {
        for (int i = 0; i < raiz->n; i++) {
            for (int j = 0; j < nivel; j++) printf("  ");
            printf("%s\n", raiz->chave[i]);
        }
        for (int i = 0; i <= raiz->n; i++)
            imprimir(raiz->filhos[i], nivel + 1);
    }
}

int carregarLivros(BTreeNode **raiz, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (!fp) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char linha[1024];
    int maiorId = 0;

    while (fgets(linha, sizeof(linha), fp)) {
        Livro *livro = (Livro *)malloc(sizeof(Livro));
        linha[strcspn(linha, "\r\n")] = '\0';
        sscanf(linha, "%d;%[^;];%[^;];%[^;];%[^;];%f;%d;%d;%[^;];%[^\n]",
               &livro->id, livro->titulo, livro->autor, livro->isbn, livro->genero,
               &livro->preco, &livro->quantidadeEstoque, &livro->anoPublicacao,
               livro->editora, livro->sinopse);

        if (livro->id > maiorId) {
            maiorId = livro->id;
        }

        char chave[200];
        snprintf(chave, sizeof(chave), "%s-%s", livro->titulo, livro->autor);
        inserir(raiz, chave, livro);
    }

    fclose(fp);
    return maiorId;
}


void salvarLivros(BTreeNode *raiz, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "w");
    if (!fp) {
        perror("Erro ao abrir o arquivo para salvar");
        exit(EXIT_FAILURE);
    }

    if (raiz != NULL) {
        for (int i = 0; i < raiz->n; i++) {
            Livro *livro = raiz->livros[i];
            fprintf(fp, "%d;%s;%s;%s;%s;%.2f;%d;%d;%s;%s\n",
                    livro->id, livro->titulo, livro->autor, livro->isbn, livro->genero,
                    livro->preco, livro->quantidadeEstoque, livro->anoPublicacao,
                    livro->editora, livro->sinopse);
        }

        for (int i = 0; i <= raiz->n; i++) {
            salvarLivros(raiz->filhos[i], nomeArquivo);
        }
    }

    fclose(fp);
}

void criarEInserirLivro(BTreeNode **raiz, int *proximoId, 
    const char *titulo, const char *autor, 
    const char *isbn, const char *genero,
    float preco, int quantidadeEstoque,
    int anoPublicacao, const char *editora,
    const char *sinopse) {

    Livro *novoLivro = (Livro *)malloc(sizeof(Livro));

    novoLivro->id = (*proximoId)++;
    strncpy(novoLivro->titulo, titulo, sizeof(novoLivro->titulo) - 1);
    strncpy(novoLivro->autor, autor, sizeof(novoLivro->autor) - 1);
    strncpy(novoLivro->isbn, isbn, sizeof(novoLivro->isbn) - 1);
    strncpy(novoLivro->genero, genero, sizeof(novoLivro->genero) - 1);
    novoLivro->preco = preco;
    novoLivro->quantidadeEstoque = quantidadeEstoque;
    novoLivro->anoPublicacao = anoPublicacao;
    strncpy(novoLivro->editora, editora, sizeof(novoLivro->editora) - 1);
    strncpy(novoLivro->sinopse, sinopse, sizeof(novoLivro->sinopse) - 1);

    novoLivro->titulo[sizeof(novoLivro->titulo) - 1] = '\0';
    novoLivro->autor[sizeof(novoLivro->autor) - 1] = '\0';
    novoLivro->isbn[sizeof(novoLivro->isbn) - 1] = '\0';
    novoLivro->genero[sizeof(novoLivro->genero) - 1] = '\0';
    novoLivro->editora[sizeof(novoLivro->editora) - 1] = '\0';
    novoLivro->sinopse[sizeof(novoLivro->sinopse) - 1] = '\0';

    char chave[200];
    snprintf(chave, sizeof(chave), "%s-%s", novoLivro->titulo, novoLivro->autor);

    inserir(raiz, chave, novoLivro);
}

int contemSubstring(const char *str, const char *substr) {
    if (!str || !substr) return 0;
    
    char strLower[256], substrLower[256];
    int i;
    for (i = 0; str[i] && i < 255; i++)
        strLower[i] = tolower(str[i]);
    strLower[i] = '\0';
    
    for (i = 0; substr[i] && i < 255; i++)
        substrLower[i] = tolower(substr[i]);
    substrLower[i] = '\0';
    
    return strstr(strLower, substrLower) != NULL;
}

void buscarPorTitulo(BTreeNode *raiz, const char *parteTitulo, Livro **resultados, int *count, int *capacity) {
    if (!raiz) return;
    
    for (int i = 0; i < raiz->n; i++) {
        if (contemSubstring(raiz->livros[i]->titulo, parteTitulo)) {
            if (*count >= *capacity) {
                *capacity *= 2;
                resultados = (Livro **)realloc(resultados, *capacity * sizeof(Livro *));
                if (!resultados) {
                    perror("Erro ao realocar memória");
                    exit(EXIT_FAILURE);
                }
            }
            resultados[*count] = raiz->livros[i];
            (*count)++;
        }
        
        if (!raiz->folha) {
            buscarPorTitulo(raiz->filhos[i], parteTitulo, resultados, count, capacity);
        }
    }
    
    if (!raiz->folha) {
        buscarPorTitulo(raiz->filhos[raiz->n], parteTitulo, resultados, count, capacity);
    }
}

Livro **buscarLivrosPorTitulo(BTreeNode *raiz, const char *parteTitulo, int *quantidade) {
    int capacity = 10;
    int count = 0;
    Livro **resultados = (Livro **)malloc(capacity * sizeof(Livro *));
    
    if (!resultados) {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }
    
    buscarPorTitulo(raiz, parteTitulo, resultados, &count, &capacity);
    
    *quantidade = count;
    return resultados;
}

int main(){
    BTreeNode *raiz = criarNo(1);

    int proximoId = 1;

    proximoId = carregarLivros(&raiz, "../backup_livros.txt") + 1;

    // Buscar livros que contenham "Guerra" no título
    int quantidade;
    Livro **resultados = buscarLivrosPorTitulo(raiz, "Guerra", &quantidade);
    
    printf("Foram encontrados %d livros:\n", quantidade);
    for (int i = 0; i < quantidade; i++) {
        printf("%d: %s (%s)\n", resultados[i]->id, resultados[i]->titulo, resultados[i]->autor);
    }

    // Inserir livro com dados fixos
    criarEInserirLivro(&raiz, &proximoId,
                      "A Arte da Guerra 6", "Sun Tzu",
                      "978-8520923445", "Estratégia",
                      39.90, 50, 500, "Editora ABC",
                      "Um clássico sobre estratégias militares.");

    // Imprimir a árvore B após a inserção
    imprimir(raiz, 0);

    // Salvar os livros de volta no arquivo
    salvarLivros(raiz, "../livros.txt");

    return 0;
}
