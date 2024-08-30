//
// Created by tiago on 25/05/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

typedef struct no{
    int chave;
    struct no *Fesq;
    struct no *Fdir;
    struct no *pai;
    int fb;
}no;

typedef struct avl{
    struct no *sentinela;
    int numElementos;
}avl;

avl *criaArvore(){
    avl *arv;

    arv = (avl*)malloc(sizeof(avl));
    if(!arv)
        return NULL;

    arv->sentinela = (no*)malloc(sizeof(no));
    arv->sentinela->Fdir = NULL;
    arv->sentinela->Fesq = NULL;
    arv->sentinela->pai = NULL;
    arv->sentinela->chave = -1000;
    arv->sentinela->fb = 0;
    arv->numElementos = 0;
    return arv;
}

int insereNo(avl *arv, int valor){
    no *n,*aux,*ant;
    n = (no*)malloc(sizeof(no));
    if(!n)
        return 0;
    n->chave = valor;
    n->Fdir = NULL;
    n->Fesq = NULL;
    n->pai = NULL;
    n->fb = 0;
    if(!arv->sentinela->Fdir){
        arv->sentinela->Fdir = n;
        n->pai = arv->sentinela;
        arv->numElementos++;
        return 1;
    }
    aux = arv->sentinela->Fdir;
    while(aux){
        ant = aux;
        if(aux->chave<=n->chave)
            aux = aux->Fdir;
        else
            aux = aux->Fesq;
    }
    if(ant->chave <= n->chave)
        ant->Fdir = n;
    else
        ant->Fesq = n;
    n->pai = ant;
    arv->numElementos++;
    atualizaFB_insercao(arv, n);
    return 1;
}
int removeNo(avl *arv, int valor){
        no *aux,*sucessor;
        int troca;
        aux = arv->sentinela->Fdir;
        if(!aux)
            return 0;
        while(aux && aux->chave != valor){
            if(aux->chave<=valor)
                aux = aux->Fdir;
            else
                aux = aux->Fesq;
        }
        if(!aux)
            return -1;
        arv->numElementos--;
        if(!aux->Fdir){
            if(aux->pai->chave < valor)
                aux->pai->Fdir = aux->Fesq;
            else
                aux->pai->Fesq = aux->Fesq;
            if(aux->Fesq)
                aux ->Fesq->pai = aux->pai;
            atualizaFB_remocao(arv,aux->pai,aux->chave);
            free(aux);
            return 1;
        }
        if(!aux->Fesq){
            if(aux->pai->chave < valor)
                aux->pai->Fdir = aux->Fdir;
            else
                aux->pai->Fesq = aux->Fdir;
            aux -> Fdir -> pai = aux->pai;
            atualizaFB_remocao(arv,aux->pai,aux->chave);
            free(aux);
            return 1;
        }
        sucessor = aux->Fdir;
        while (sucessor->Fesq)
        {
            sucessor = sucessor->Fesq;
        }

        aux->chave = sucessor->chave;

        if (sucessor->pai->Fesq == sucessor) {
            sucessor->pai->Fesq = sucessor->Fdir;
        } else {
            sucessor->pai->Fdir = sucessor->Fdir;
        }
        if (sucessor->Fdir) {
            sucessor->Fdir->pai = sucessor->pai;
        }
        atualizaFB_remocao(arv,sucessor->pai,sucessor->chave);
        free (sucessor);
        return 1;
    }

void imprimeOrdem(no *raiz){
    if (raiz){
        imprimeOrdem(raiz->Fesq);
        printf("%d - %d - %d\n",raiz->chave, raiz->pai->chave, raiz->fb);
        imprimeOrdem(raiz->Fdir);
    }
}

void imprimePreOrdem(no *raiz){
    if (raiz){
        printf("%d - %d - %d\n",raiz->chave, raiz->pai->chave, raiz->fb);
        imprimePreOrdem(raiz->Fesq);
        imprimePreOrdem(raiz->Fdir);
    }
}

no *getRaiz(avl *arv){
    return arv->sentinela->Fdir;
}

int getNumElementos(avl *arv){
    return arv->numElementos;
}

int processaCarga(avl *arv, char *nomeArquivo, int tipo){
    FILE *arq;
    int aux, ret;
    arq = fopen(nomeArquivo,"r");
    if(!arq) {
        return -2;
    }
    if (tipo == 1) {
        while (!feof(arq)) {
            if (fscanf(arq, "%d", &aux) == 1) {
                ret = insereNo(arv, aux);
            }
        }
    }
    if (tipo == 2){
        while (!feof(arq)) {
            if (fscanf(arq, "%d", &aux) == 1) {
                ret = removeNo(arv, aux);
            }
        }
    }
    fclose(arq);
    return ret;
}

void atualizaFB_insercao(avl *arv, no *novoNo){
    no *aux;
    aux = novoNo;
    do {
        if (aux->chave < aux->pai->chave) {
            aux->pai->fb--;
        } else {
            aux->pai->fb++;
        }
        aux = aux->pai;
    }while(aux != arv->sentinela->Fdir && aux->fb != -2 && aux->fb !=2 && aux->fb!=0);
    if (aux->fb == -2 || aux->fb == 2)
        balanceamento(arv,aux);
}

void atualizaFB_remocao(avl *arv, no *s, int chaveRemovida){
    no *aux;
    aux = s;
    if (aux == arv->sentinela)
        return;
    if (chaveRemovida < aux->chave)
        aux->fb++;
    else{
        aux->fb--;
    }
    while(aux != arv->sentinela->Fdir && aux->fb == 0){
        aux = aux->pai;
        if (aux == arv->sentinela)
            return;
        if (chaveRemovida < aux->chave)
            aux->fb++;
        else {
            aux->fb--;
        }
    }
    if (aux->fb == 2 || aux->fb == -2){
        balanceamento(arv,aux);
        if(aux->pai != arv->sentinela && aux->pai->fb == 0)
            atualizaFB_remocao(arv,aux->pai->pai,chaveRemovida);
    }
}

void balanceamento(avl *arv, no *noDesbal){
    no *filho;
    int fbneto, fbfilho;
    if (noDesbal->fb == 2){
        filho = noDesbal->Fdir;
        if (filho->fb == -1){
            fbneto = filho->Fesq->fb;
            filho->Fesq->fb = 0;
            rotacaoDir(filho);
            rotacaoEsq(noDesbal);
            if (fbneto == 0){
                noDesbal->fb = 0;
                filho->fb =0;
            }else if(fbneto == -1) {
                noDesbal->fb = 0;
                filho->fb = 1;
            }else{
                noDesbal->fb =-1;
                filho->fb = 0;
            }
        }else{
            fbfilho = filho->fb;
            rotacaoEsq(noDesbal);
            if (fbfilho == 0){
                filho->fb = -1;
                noDesbal->fb = 1;
            }else{
                filho->fb = 0;
                noDesbal->fb = 0;
            }
        }
    }else{
        filho = noDesbal->Fesq;
        if (filho->fb == 1){
            fbneto = filho->Fdir->fb;
            filho->Fdir->fb = 0;
            rotacaoEsq(filho);
            rotacaoDir(noDesbal);
            if (fbneto == 0){
                noDesbal->fb = 0;
                filho->fb =0;
            }else if(fbneto == -1) {
                noDesbal->fb = 1;
                filho->fb = 0;
            }else{
                noDesbal->fb =0;
                filho->fb = -1;
            }
        }else{
            fbfilho = filho->fb;
            rotacaoDir(noDesbal);
            if (fbfilho == 0){
                filho->fb = 1;
                noDesbal->fb = -1;
            }else{
                filho->fb = 0;
                noDesbal->fb = 0;
            }
        }
    }
}

void rotacaoDir(no *noDesbal){
    no *filho;
    filho = noDesbal->Fesq;
    noDesbal->Fesq = filho->Fdir;
    if(filho->Fdir)
        filho->Fdir->pai = noDesbal;
    filho->pai = noDesbal->pai;
    if (noDesbal == noDesbal->pai->Fesq)
        noDesbal->pai->Fesq = filho;
    else
        noDesbal->pai->Fdir = filho;
    filho->Fdir = noDesbal;
    noDesbal->pai = filho;
}

void rotacaoEsq(no *noDesbal){
    no *filho;
    filho = noDesbal->Fdir;
    noDesbal->Fdir = filho->Fesq;
    if(filho->Fesq)
        filho->Fesq->pai = noDesbal;
    filho->pai = noDesbal->pai;
    if (noDesbal == noDesbal->pai->Fesq)
        noDesbal->pai->Fesq = filho;
    else
        noDesbal->pai->Fdir = filho;
    filho->Fesq = noDesbal;
    noDesbal->pai = filho;
}