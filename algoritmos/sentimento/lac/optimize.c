//da um peso para cada atributo
//o peso de uma regra eh a media dos pesos de suas features
//fica tentando pesos ate atingir os melhores resultados

//primeiro: gera-se as regras para uma instancia de teste
//depois: da um peso para cada atributo
//depois: calcula-se o peso de cada regra
//depois: ve quanto da de acuracia na validacao
//depois: repete-se o processo, variando o peso das features
//depois: se o resultado melhorar, troca-se os pesos otimos

//primeiro: otimiza a funcao de score sum(w(r)*theta(r))
//ou seja, tem um peso para cada regra. para evitar explosao
//faz o peso das regras mesticas ser a media dos pesos dos atomos

//desafio: o problema nao eh convexo

#include "rule.h"
#include "optimize.h"

int optimize() {
	return(0);
}
