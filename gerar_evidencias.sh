#!/bin/bash
# gerar_evidencias.sh
# Roda todos os casos de teste e registra saida + exit code em evidencias.log.
# Uso: ./gerar_evidencias.sh
#SCRIPT DE CHECAGEM


LOG=evidencias.log
BIN=./scheduler

# Zera o log e escreve cabecalho.
{
    echo "======================================================"
    echo " EVIDENCIAS DE TESTE - Escalonador rate/edf"
    echo " Gerado em: $(date '+%Y-%m-%d %H:%M:%S')"
    echo " SO: $(uname -srm)"
    echo "======================================================"
} > "$LOG"

# Funcao auxiliar: roda um comando, registra descricao, saida e exit code.
run() {
    desc="$1"; shift
    {
        echo ""
        echo "------------------------------------------------------"
        echo "TESTE: $desc"
        echo "CMD:   $*"
        echo "------ saida (stdout + stderr) ------"
        "$@" 2>&1
        echo "------ exit code: $? ------"
    } >> "$LOG"
}

# ---------- EXECUCOES VALIDAS ----------
run "rate no voo.txt (execucao valida)"  $BIN rate voo.txt
run "edf no voo.txt (execucao valida)"   $BIN edf  voo.txt

# Registra o conteudo dos .out gerados acima.
{
    echo ""
    echo "------ conteudo de rate_jlogb.out ------"
    cat rate_jlogb.out 2>&1
    echo ""
    echo "------ conteudo de edf_jlogb.out ------"
    cat edf_jlogb.out 2>&1
} >> "$LOG"

# ---------- TRATAMENTO DE ERRO ----------
run "numero de argumentos: nenhum"        $BIN
run "numero de argumentos: faltando arq"  $BIN rate
run "algoritmo invalido"                  $BIN xpto voo.txt
run "arquivo inexistente"                 $BIN rate naoexiste.txt
run "campo faltando"                      $BIN rate t_falta.txt
run "valor nao numerico"                  $BIN rate t_naonum.txt
run "valor nao positivo (zero)"           $BIN rate t_zero.txt
run "deadline > periodo (D > P)"          $BIN rate t_dmaior.txt
run "burst > deadline (C > D)"            $BIN rate t_cmaiord.txt

# ---------- VERIFICACAO: erros nao criam .out ----------
{
    echo ""
    echo "------------------------------------------------------"
    echo "VERIFICACAO: nenhum .out indevido apos os casos de erro"
    echo "CMD:   ls *.out"
    echo "------ saida ------"
    ls *.out 2>&1
    echo "------------------------------------------------------"
} >> "$LOG"

echo "Pronto. Veja $LOG"