//Aluno: William Rodrigues da Silva | RA99514
//Trabalho realizado em 2018 para disciplina de ORD

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void importaDados(FILE *entrada, FILE *saida){

    char buffer[200], caracter[2];
    int i, tamCampo, verificaEOF=0, cabeca=(-1);

    //Cria cabeçalho do arquivo (Cabeça da LED)
    fwrite(&cabeca, sizeof(int), 1, saida);

    verificaEOF = fgetc(entrada);
    while(verificaEOF != EOF){
        fseek(entrada,-1,SEEK_CUR);
        i=0;
        buffer[0] = '\0';
        while(i<4){
            fscanf(entrada, "%c", &caracter[0]);
            caracter[1]='\0';
            strcat(buffer, caracter);
            if(caracter[0]=='|'){
                i++;
            }
        }
        //printf("\n%s", buffer); //Print de teste no console
        tamCampo = strlen(buffer);
        fwrite(&tamCampo, sizeof(int), 1, saida);
        fwrite(buffer, tamCampo, 1 ,saida);

        verificaEOF = fgetc(entrada);
    }
    printf("\nImportacao realizada com Sucesso!");
}


int getRegistro(FILE *arq, char buffer[]){

    int tamanhoRegistro;

    if(fread(&tamanhoRegistro, sizeof(int), 1, arq)==0){
        return 0;
    }
    fread(buffer, tamanhoRegistro, 1, arq);
    buffer[tamanhoRegistro] = '\0';
    //printf("\nBuffer retornado: %s", buffer); //Print de teste no console
    return tamanhoRegistro;
}


void removeRegistro(FILE* arq, char inscricao[]){

    int encontrado=0, tamanhoRegistro, cabecaLED;
    char buffer[200], *resultadoBusca;
    fflush(arq);
    fseek(arq, 0, SEEK_SET);
    fread(&cabecaLED, sizeof(int), 1, arq);
    printf("\nCabeca LED: %d", cabecaLED); //Print de teste no console
    fseek(arq,4,SEEK_SET);

    while(!encontrado){
        tamanhoRegistro=getRegistro(arq, buffer);
        if(tamanhoRegistro>0){
            //printf("\n%s", buffer); //Print de teste no console
            resultadoBusca = strtok(buffer, "|");
            if(strcmp(inscricao, resultadoBusca)==0){
                encontrado = 1;
            }
        }else break;
    }
    if(encontrado){
        fseek(arq, -tamanhoRegistro-4, SEEK_CUR);
        int offset = ftell(arq);
        printf("\nOffset do registro: %d", offset); //Print de teste no console
        fseek(arq, 4, SEEK_CUR);
        fprintf(arq, "*|");
        fprintf(arq, "%d|", cabecaLED);
        fflush(arq);
        fseek(arq, 0, SEEK_SET); //Posiciona no cabeçalho novamente
        fwrite(&offset, sizeof(int), 1, arq);
        printf("\nRegistro de matricula %s removido.", inscricao);
        printf("\nCabeca LED atualizada para %d.", offset);
    }else{
        printf("\nNenhum resultado encontrado para inscricao: \"%s\"", inscricao);
    }

}


void insereRegistro(FILE *arq, char buffer[], int tamRegistro){

    int cabecaLED, offsetProx, copiaOffset, tamDisponivel, fimLED=0;

    fseek(arq, 0, SEEK_SET);
    fread(&cabecaLED, sizeof(cabecaLED), 1, arq);
    offsetProx = cabecaLED;
    //printf("\nCabeca LED: %d", cabecaLED); //Print de teste no console

    if(cabecaLED==(-1)){ //Caso LED Vazia
        printf("\nLED vazia, registro inserido no final do arquivo.");
        fseek(arq, 0, SEEK_END);//Insere no fim do arquivo
        fwrite(&tamRegistro, sizeof(int), 1, arq);
        fwrite(buffer, tamRegistro, 1, arq);
    }else{
        fseek(arq, cabecaLED, SEEK_SET);
        fread(&tamDisponivel, sizeof(tamDisponivel), 1, arq);
        while(fimLED==0){
            if(tamDisponivel>=tamRegistro){
                printf("\nEncontrou espaco adequado na LED");
                printf("\nRegistro inserido no offset %d.", offsetProx);
                //Guarda o valor para atualizar a cabeça LED
                fseek(arq, 2, SEEK_CUR); //pular *|
                copiaOffset = offsetProx;
                fscanf(arq, "%d", &offsetProx);
                //printf("\nOffset prox: %d", offsetProx); //Print de teste no console
                //Sobrescreve os dados do novo registro
                fseek(arq, copiaOffset, SEEK_SET);
                fwrite(&tamRegistro, sizeof(int), 1, arq);
                fwrite(buffer, tamRegistro, 1, arq);
                //Grava o novo valor na cabeça LED
                fseek(arq, 0, SEEK_SET);
                fwrite(&offsetProx, sizeof(int), 1, arq);

                return;
            }
            printf("\nEspaco insuficiente para inserir registro");
            printf("\nEncontrando proximos espacos na LED...");
            fseek(arq, 2, SEEK_CUR);
            fscanf(arq, "%d", &offsetProx);
            //printf("\nOffset prox: %d", offsetProx); //Print de teste no console

            if(offsetProx==(-1)){
                fimLED=1;
            }

            fseek(arq, offsetProx, SEEK_SET);
            fread(&tamDisponivel, sizeof(tamDisponivel), 1, arq);

        }
        printf("\nNao encontrou nenhum espaco adequado na LED");
        printf("\nRegisto inserido no final do arquivo.");
        fseek(arq, 0, SEEK_END);//Insere no fim do arquivo
        fwrite(&tamRegistro, sizeof(int), 1, arq);
        fwrite(buffer, tamRegistro, 1, arq);
    }


}


void recuperarRegistro(FILE *arq, char inscricao[]){

    int encontrado=0, tamanhoRegistro, cabecaLED;
    char buffer[200], *resultadoBusca;
    fflush(arq);
    fseek(arq, 0, SEEK_SET);
    fread(&cabecaLED, sizeof(int), 1, arq);
    fseek(arq,4,SEEK_SET);

    while(!encontrado){
        tamanhoRegistro=getRegistro(arq, buffer);
        if(tamanhoRegistro>0){
            //printf("\n%s", buffer); //Print de teste no console
            resultadoBusca = strtok(buffer, "|");
            if(strcmp(inscricao, resultadoBusca)==0){
                encontrado = 1;
            }
        }else break;
    }
    if(encontrado){
        printf("\nResultado da Busca:\n");
        printf("| %s |", resultadoBusca);
        resultadoBusca = strtok(NULL, "|");
        while(resultadoBusca!=NULL){
            printf(" %s |", resultadoBusca);
            resultadoBusca = strtok(NULL, "|");
        }
    }else{
        printf("\nNenhum resultado encontrado para inscricao: \"%s\"", inscricao);
    }
}


void solicitaRemocao(FILE *arq){

    char inscricao[10];

    fflush(stdin);
    printf("\nREMOVER Registro");
    printf("\nDigite a inscricao para remover: ");
    scanf("%s", &inscricao);
    removeRegistro(arq, inscricao);
}


void solicitaInsercao(FILE *arq){
    int tamRegistro;
    char inscricao[10], nome[50], curso[20], nota[10], buffer[100];

    buffer[0] = '\0';

    fflush(stdin);
    printf("\nINSERIR Registro");
    printf("\nDigite o numero de inscricao: ");
    scanf("%s", &inscricao);
    strcat(buffer, inscricao);
    strcat(buffer, "|");

    fflush(stdin);
    printf("Digite o nome: ");
    scanf("%s", &nome);
    strcat(buffer, nome);
    strcat(buffer, "|");

    fflush(stdin);
    printf("Digite o curso: ");
    scanf("%s", &curso);
    strcat(buffer, curso);
    strcat(buffer, "|");

    fflush(stdin);
    printf("Digite a nota: ");
    scanf("%s", &nota);
    strcat(buffer, nota);
    strcat(buffer, "|");

    tamRegistro = strlen(buffer);
    insereRegistro(arq, buffer, tamRegistro);
}


void solicitaRecuperarRegistro(FILE *arq){
    char inscricao[10];

    fflush(stdin);
    printf("\nBUSCAR Registro");
    printf("\nDigite a inscricao para buscar: ");
    scanf("%s", &inscricao);
    recuperarRegistro(arq, inscricao);
}


void exibirMenu(){
    printf("\n\n  1 - Importar dados");
    printf("\n  2 - Editar arquivo existente");
    printf("\n  3 - Inserir registro");
    printf("\n  4 - Remover registro");
    printf("\n  5 - Buscar um registro");
    printf("\n  0 - Sair\n");
}


void exibirMenuInicial(){
    printf("\n\n  1 - Importar dados");
    printf("\n  2 - Editar arquivo existente");
    printf("\n  0 - Sair\n");
}


int main(){

    int opcao=1, cabecaLED, arquivoAberto=0;
    char nomeArqEntrada[50], nomeArqSaida[50];
    FILE *entrada, *saida;

    strcpy(nomeArqSaida, "Nenhum arquivo aberto.");
    printf("Gerenciamento de Registros de tamanho variavel");

    while(opcao!=0){

        printf("\n\n---------------------------------------------------");
        printf("\nInfo: Arquivo sendo editado: %s", nomeArqSaida);
        if(arquivoAberto==1) exibirMenu();
        else exibirMenuInicial();
        printf("\n---------------------------------------------------");
        printf("\nOpcao: ");
        scanf("%d", &opcao);
        fflush(stdin);

        switch(opcao){

        case 1:
            fflush(stdin);
            printf("\nDigite o nome do arquivo de origem: ");
            scanf("%s", &nomeArqEntrada);
            printf("Digite o nome do arquivo de destino: ");
            scanf("%s", &nomeArqSaida);
            entrada = fopen(nomeArqEntrada, "rb");
            if(entrada==NULL){
                printf("\nERRO: Arquivo de Origem \"%s\" nao encontrado.", nomeArqEntrada);
                strcpy(nomeArqSaida, "Nenhum arquivo aberto.");
                arquivoAberto=0;
                break;
            }
            saida = fopen(nomeArqSaida, "wb");
            importaDados(entrada, saida);
            fclose(saida);
            fclose(entrada);
            arquivoAberto=1;
            break;

        case 2:
            fflush(stdin);
            printf("\nDigite o nome do arquivo existente: ");
            scanf("%s", &nomeArqSaida);
            arquivoAberto=1;
            break;

        case 3:
            if(arquivoAberto==1){
                saida = fopen(nomeArqSaida, "rb+");
                solicitaInsercao(saida);
                fclose(saida);
            }else printf("\nERRO: Nenhum arquivo aberto para edicao.");
            break;

        case 4:
            if(arquivoAberto==1){
                saida = fopen(nomeArqSaida, "rb+");
                solicitaRemocao(saida);
                fclose(saida);
            }else printf("\nERRO: Nenhum arquivo aberto para edicao.");
            break;

        case 5:
            if(arquivoAberto==1){
                saida = fopen(nomeArqSaida, "rb+");
                solicitaRecuperarRegistro(saida);
                fclose(saida);
            }else printf("\nERRO: Nenhum arquivo aberto para edicao.");
            break;

        case 0:
            printf("\nFim do programa.");
            break;

        default:
            printf("\nERRO: Opcao \"%d\" invalida.", opcao);
            break;

        }
    }
    return 0;
}
