# SO II - Relatório do Trabalho sobre Métodos de Busca - Parte 1

- Semestre: 2015-2
- Aluno: Quenio Cesar Machado dos Santos
- Matrícula: 14100868

## Enunciado

Implementação do jogo Gomoku usando o algoritmo de busca MinMax com podas. Nesta primeira parte do trabalho será avaliado:
- a implementação do modelo de estados e do jogo - ver código-fonte em anexo;
- e a definição das funções utilidade e heurísticas - ver seção Função de Pontuação (Utilidade) abaixo.

## Função de Pontuação (Utilidade)

O algoritmo min-max navegada a árvore de estados do jogo para definir a utilidade de cada tabuleiro (estado do jogo), ou seja, para definir um valor (ou pontuação) que exprime quão próximo da vitória está um estado do tabuleiro relativamente a outros estados do tabuleiro que podem ser jogados.

Quando chega aos nós-folha desta árvore, ou seja, a um estado final do jogo, é necessário definir através de uma pontuação se um nó-folha é um resultado de vitória, perda ou empate para o jogador.

Porém, num jogo como o Gomoku, a árvore de estados de jogo é simplesmente muito grande para ser percorrida em tempo hábil para se fazer uma jogada. Portanto, é necessário interromper a busca em níveis superiores da árvore ou ainda eliminar alguns ramos da árvore de busca.

Para isto, é necessário definir algumas heurísticas que possam usar apenas o estado de jogo, representado por um único nodo da árvore, para se calcular uma pontuação que se aproxime do resultado que seria alcançado caso a sub-árvore daquele nodo fosse percorrida.

Desta forma, a função de utilidade (abaixo chamada "score") é aquela que retorna a pontuação de um nó quando este é um nó terminal ou quando se alcançou o nível limite de busca na árvore.

Pode-se definí-la de forma abstrata com o código-fonte em "C" abaixo:

```C++
int score(GameNode node)
{
  return isGameOver(node) ? terminalScore(node) : heuristicScore(node);
}
```

## Vitória, Derrota ou Empate

Para os nodos da árvore no estado terminal do jogo, a função utilidade precisa gerar apenas três valores de pontuação:
- um valor de vitória;
- um valor de derrota;
- e um valor de empate.

Todos as outras pontuações geradas pela função utilidade, se não foram calculadas a partir de nodos terminais da árvore, precisam necessariamente serem menores que o valor de vitória, pois são apenas estimativas; não são baseados num estado de vitória do jogo.

Para o jogo Gomoku, fixamos os seguintes valores:
- vitória: +500;
- derrota: -500;
- empate: 0.

Este valor foi escolhido, pois:
- é o número de casas do tabuleiro necessários para um jogador ganhar o jogo, multiplicado por 100.
- o número de casas é usado para calcular a pontuação baseada em heurísticas, como descrito nas próximas seções.

## Heurísticas

Como foi explicado nas seções acima, quando não é possível percorrer uma árvore de jogo até seus nodos terminais, é preciso utilizar algumas heurísticas (desenvolvidas com a experiência de jogar) para se calcular a pontuação de nodos intermediários.

Aqui estão algumas heurísticas que conseguimos determinar ao jogar Gomoku:
- Campo Limpo: quando se começa jogando, a fim de maximizar as opções de jogada, sempre é bom começar o jogo numa casa que tenha pelo menos 4 espaços em todas as direções.
- Pontas Livres: sempre escolha aumentar uma linha que tem pontas livres do que uma linha que já está fechada em um dos lados, pois se cria a possibilidade de fechar em qualquer uma das pontas.
- Triangulação: usar o mesmo marcador em linhas ortgonais ou diagonais cria mais opções de vitória com um número menor de jogadas e exige mais defesas do adversário, diminuindo o número de jogadas ofensivas que ele pode fazer.
- Tracejando: Formar linhas não consecutivas (antes de fechá-las) pode torná-las imperceptíveis para o adersário se este não prestar atenção.
- Sentinela: É sempre melhor impedir que o adversário consiga formar uma linha de quatro marcadores com pontas livres do que tentar avançar numa linha própria, pois não há como se recuperar desta situação.
- Bloqueio Amplo: se puder bloquear em ambas as pontas de uma linha do adversário, escolha a ponta que também bloqueia outras linhas. Assim você consegue defender-se em duas situações com apenas uma jogada.

As heurísticas definidas acima precisam ser traduzidas em uma função matemática que recebe o estado atual do jogo (nó da árvore) como entrada e retorna uma pontuação que exprime o quão próximo de uma vitória, derrota ou empate se encontra o jogador a partir do tabuleiro que se apresenta.

Além disso, esta pontuação precisa priorizar as heuristícas de acordo com cada situação, ou estado do tabuleiro, visto que elas podem aplicar-se igualmente em uma determinada posição do tabuleiro.

## A Pontuação Baseada em Heurísticas

Nesta seção, definiremos a função utilidade baseada em heurísticas que implementaremos na segunda parte do trabalho. Primeiramente, vamos definir o algoritmo. Posteriormente, vamos avaliá-lo de acordo com cada heurística.

Informalmente, o algoritmo pode ser descrito de forma bem resumida nos três passos a seguir:
1. Para cada direção do tabuleiro, haverá uma pontuação associada à casa onde será feita potentialmente a próxima jogada. Esta pontuação é calculada usando o estado das casas adjacentes naquela direção do tabuleiro, considerando se a casa da próxima jogada seja sua ou do adversário.
2. O valor calculado para cada tupla (direção, jogador) é então comparado aos demais. O maior valor é considerado a pontuação associada ao estado do tabuleiro quando aquela jogada é efetuada pelo jogador.
3. Se houver mais de um valor máximo retornado em direções diferentes, se adiciona "1" a pontuação para cada valor máximo adicional.

Em "C", pode-se definir o algoritmo acima de forma abstrata:

```C++
enum PlayerMaker
{
  X, // Computer
  O // Human
}
enum Direction { North, Northeast, East, Southeast, South, Southwest, West, Northwest };

int score(GameNode node)
{
  int max = MIN_INT;
  int extra = 0;
  for (Direction direction = North; direction <= Northwest; direction++)
  {
    int score = score(node, direction, PlayerMaker::X);
    if (score == max)
    {
      extra++;
    }
    else if (score > max)
    {
      max = score;
      extra = 0;
    }

    int score = score(node, direction, PlayerMaker::O);
    if (score == max)
    {
      extra++;
    }
    else if (score > max)
    {
      max = score;
      extra = 0;
    }
  }
  return max + extra;
}
```

O cálculo da pontuação de cada direção do tabuleiro descrita no passo 1 acima pode ser descrito em "C":

```C++
int score(GameNode node, Direction direction, PlayerMarker marker)
{
  int score = 100;
  int step = 1;
  int interval = 1;
  Slot current = node.slot;  
  while (current != nullptr && interval < 5)
  {
    previous = current;
    current = neighbor(current, direction, step);
    if (current == nullptr || current.marker != marker) // bloqueado
    {
      if (step == 1)
      {
        // Já bloqueado na posição de jogada. Nada para buscar nesta direção.
        current = nullptr;
      }
      else if (step > 1)
      {
        step = -1; // Chegou-se ao final da linha; segue na direção oposta.
        current = node.slot
      }
      score--; // Uma linha bloqueada deve valer menos que uma linha livre.
    }
    else if (current.marker == marker)
    {
      score += 100; // Pontuação cheia pois esta casa já é marcada.
      step++; // Segue adiante no mesmo sentido da direção.
      interval++;
    }
    else // if (current.empty)
    {
        score += 50; // Meia-pontuação pois esta casa é apenas uma possibilidade.
        step++; // Segue adiante no mesmo sentido da direção.
        interval++;
    }
    if (previous != node.slot && previous.marker != current.marker)
    {
      score++; // Tracejados tem pontos mais altos porque eles podem confundir o adversário.
    }
  }

  if (interval == 5)
  {
    return score; // há potencial nesta direção
  }
  else
  {
    return 0; // não há casas suficientes nesta direção para fechar o jogo.
  }
}
```

O algoritmo acima está basicamente fazendo o seguinte:
1. Cada casa adjacente é percorrida numa direção até se alcançar um intervalo de 5 casas.
2. Para cada casa vazia, somasse 50 à pontuação inicial, pois se trata de uma marca em potencial para o jogador, mas pode não ser concretizada.
3. Para cada casa marcada com o marcador de jogada passado como paramêtro, somasse 100 à pontuação, pois o jogador já garantiu esta casa para a formação de sua linha.
4. Casa marcada com o marcador do adversário (ou se chegou no final do tabuleiro) faz a busca seguir no sentido contrário e perdesse um ponto por ser uma linha bloqueada em uma das pontas. Se uma direção for bloqueada já na casa da jogada, então o sentido contrário nem é percorrido, porque já vai ser coberto pela pontuação da direção oposta.
5. Ganhasse um ponto caso a linha seja tracejada. Esta deve valer mais que uma linha contínua porque ela pode confundir o adversário fazendo não perceber que se está perto de fechar uma linha.

Observe que este algoritmo percorre todas as direções e somente escolhe a pontuação maior, ao invés de somar o valor de cada direção. Isto é porque este algoritmo tenta de certa forma simular o que aconteceria caso a árvore de busca fosse percorrida pelo algoritmo min-max, onde também se escolhe a pontuação máxima dos nós-filho. Pode-se dizer que este algoritmo é uma localização da busca min-max sem precisar percorrer a árvore de estados do tabuleiro.

## Análise da Efetividade da Função de Pontuação

Dado o algoritmo acima, vamos analisar como ele se comporta diante de alguns cenários do jogo Gomoku abordados pelas heurísticas definidas anteriormente.

### Campo Limpo

Num cenário de começo de jogo quando o campo está limpo, uma casa no centro do tabuleiro vai atingir uma pontuação mais alta que uma casa nas bordas do tabuleiro, pois uma casa do centro pode pontuar em todas as direções.

### Pontas Livres

Uma casa que forma uma linha com pontas livres irá pontuar mais que uma casa que possui bloqueio em algumas das pontas, porque a última será penalidade com a diminuição da pontuação quando um bloqueio for encontrado.

### Triangulação

Uma casa que faz parte de várias linhas em direções diferentes vai pontuar mais alto que uma casa participando de apenas uma linha, porque para cada valor máximo repetido numa direção geração pontuação de "gratificação" para aquela casa.

### Tracejando

Linhas com alternância de marcador e casas vazias são gratificas e pontuam mais que uma linha com o mesmo número de marcadores em sequência.

### Sentinela

O cenário em que o adversário estas prestes a completar uma linha de quatro casas com duas pontas livres é coberto por aspectos do algoritmo descritos abaixo:
- Primeiramente, a pontuação das jogadas simuladas com o adversário jogando na mesma posição são comparadas na função de utilidade da mesma forma que uma jogada do computador. Desta forma, se a jogada do adversário se torna mais vantajosa a este do que outra jogada do computador, então o computador jogará na casa que traria vantagem ao adversário para lhe impedir de fazê-la.
- Além disso, como descrito na seção Pontas Livres, as jogadas que forem bloqueadas em uma das pontas serão penalizadas, portanto a jogada do adversário que tem pontas livres terá pontuação maior que qualquer outra jogada do computador que tenha bloqueios. Assim o computador vai acabar jogando na posição queria traria pontas livres para o adversário.

### Bloqueio Amplo

Assim como foi descrito na Triangulação, cada valor máximo repetido em uma casa é gratificado, mesmo quando calculando um cenário de jogada do adversário. Assim, uma posição de múltiplos bloqueios terá precedência sobre uma posição onde o computador fazendo um único bloqueio.

### Combinação de Cenários

Na segunda parte do trabalho, a intenção é fazer também uma análise da combinação de diferentes cenários (por exemplo, Bloqueio Amplo vs Pontas Livres) e verificar se o algoritmo ainda continua fazendo as melhores escolhas.

## Otimizações

Visto que a busca do min-max é proibitiva num tabuleiro 15x15, devido ao tamanho da árvore gerada para todos os estados do jogo, é importante considerar formas de otimização que permitam diminuir a árvore de busca. Isto tornaria viável buscas em níveis mais baixos da árvore, quem sabe até permitindo a busca alcançar os nós terminais, assim evitando o uso das heurísticas da função de utilidade.

### Podas com Alpha & Beta

O algoritmo min-max que mantém valores de alpha e beta para cada nó permite que ramos inteiros da árvore de busca possam ser ignorados, pois estes não poderiam em tese trazer resultados melhores do que já se alcançou com os ramos já pesquisados.

Na parte 2 do trabalho, a intenção é modificar a implementação inicial do algoritmo min-max, introduzindo podas, e verificar se foi o suficiente para aumentar a profundidade da busca.

### Foco em Áreas do Tabuleiro

Outra abordagem que tentaremos na segunda parte do trabalho é limitar a busca a uma área menor do tabuleiro. Se o jogo num determinado momento se concentra na área central do tabuleiro, por exemplo, então a busca poderia criar uma área central menor (vamos dizer, de 8x8) e então fazer a busca somente nos estados gerados dentro desta área.

No início do jogo, quando ainda existem poucas áreas marcadas, esta abordagem poder ser muito efetiva. Porém, a medida que o adversário começa a marcar áreas novas do tabuleiro, é necessário avaliá-las. Talvez seja possível manter várias áreas de busca separadas, dependendo da distância entre elas.

----------
