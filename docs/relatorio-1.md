# SO II - Relatório do Trabalho sobre Métodos de Busca - Parte 1

- Semestre: 2015-2
- Aluno: Quenio Cesar Machado dos Santos
- Matrícula: 14100868

# Enunciado

Implementação do jogo Gomoku usando o algoritmo de busca MinMax com podas. Nesta primeira parte do trabalho será avaliado:
- a implementação do modelo de estados e do jogo - ver código-fonte em anexo;
- e a definição das funções utilidade e heurísticas - ver seção Função de Pontuação (Utilidade) abaixo.

# Função de Pontuação (Utilidade)

O algoritmo min-max navegada a árvore de estados do jogo para definir a utilidade de cada tabuleiro (estado do jogo), ou seja, para definir um valor (ou pontuação) que exprime quão próximo da vitória está um estado do tabuleiro relativamente a outros estados do tabuleiro que podem ser jogados.

Quando chega aos nós-folha desta árvore, ou seja, a um estado final do jogo, é necessário definir através de uma pontuação se um nó-folha é um resultado de vitória, perda ou empate para o jogador.

Porém, num jogo como o Gomoku, a árvore de estados de jogo é simplesmente muito grande para ser percorrida em tempo hábil para se fazer uma jogada. Portanto, é necessário interromper a busca em níveis superiores da árvore ou ainda eliminar alguns ramos da árvore de busca.

Para isto, é necessário definir algumas heurísticas que possam usar apenas o estado de jogo, representado por um único nodo da árvore, para se calcular uma pontuação que se aproxime do resultado que seria alcançado caso a sub-árvore daquele nodo fosse percorrida.

Desta forma, a função de utilidade (abaixo chamada "score") é aquela que retorna a pontuação de um nó quando este é um nó terminal ou quando se alcançou o nível limite de busca na árvore.

Pode-se definí-la de forma abstrata com o código-fonte em "C" abaixo:

```
int score(GameNode node)
{
  return isGameOver(node) ? terminalScore(node) : heuristicScore(node);
}
```

# Vitória, Derrota ou Empate

Para os nodos da árvore no estado terminal do jogo, a função utilidade precisa gerar apenas três valores de pontuação:
- um valor de vitória;
- um valor de derrota;
- e um valor de empate.

Todos as outras pontuações geradas pela função utilidade, se não foram calculadas a partir de nodos terminais da árvore, precisam necessariamente serem menores que o valor de vitória, pois são apenas estimativas; não são baseados num estado de vitória do jogo.

Para o jogo Gomoku, fixamos os seguintes valores:
- vitória: +5;
- derrota: -5;
- empate: 0.

Este valor foi escolhido, pois:
- é o número de casas do tabuleiro necessários para um jogador ganhar o jogo.
- o número de casas é usado para calcular a pontuação baseada em heurísticas, como descrito nas próximas seções.

# Heurísticas

Como foi explicado nas seções acima, quando não é possível percorrer uma árvore de jogo até seus nodos terminais, é preciso utilizar algumas heurísticas (desenvolvidas com a experiência de jogar) para se calcular a pontuação de nodos intermediários.

Aqui estão algumas heurísticas que conseguimos determinar ao jogar Gomoku:
- Campo Limpo: quando se começa jogando, a fim de maximizar as opções de jogada, sempre é bom começar o jogo numa casa que tenha pelo menos 4 espaços em todas as direções.
- Pontas Livres: sempre escolha aumentar uma linha que tem pontas livres do que uma linha que já está fechada em um dos lados, pois se cria a possibilidade de fechar em qualquer uma das pontas.
- Triangulação: usar o mesmo marcador em linhas ortgonais ou diagonais cria mais opções de vitória com um número menor de jogadas e exige mais defesas do adversário, diminuindo o número de jogadas ofensivas que ele pode fazer.
- Tracejando: Formar linhas não consecutivas (antes de fechá-las) pode torná-las imperceptíveis para o adersário se este não prestar atenção.
- Sentinela: É sempre melhor impedir que o adversário consiga formar uma linha de quatro marcadores com pontas livres do que tentar avançar numa linha própria, pois não há como se recuperar desta situação.
- Bloqueio Amplo: se puder bloquear em ambas as pontas de uma linha do adversário, escolha a ponta que também bloqueia outras linhas. Assim você consegue defender-se em duas situações com apenas uma jogada.

# A Pontuação Baseada em Heurísticas

As heurísticas definidas acima precisam ser traduzidas em uma função matemática que recebe o estado atual do jogo (nó da árvore) como entrada e retorna uma pontuação que exprime o quão próximo de uma vitória, derrota ou empate se encontra o jogador a partir do tabuleiro que se apresenta.

Além disso, esta pontuação precisa priorizar as heuristícas de acordo com cada situação, ou estado do tabuleiro, visto que elas podem aplicar-se igualmente em uma determinada posição do tabuleiro.