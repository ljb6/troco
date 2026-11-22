#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

const int INFINITO = 1e9;

// Limite de chamadas recursivas: sem memorizacao o numero de chamadas
// cresce exponencialmente, entao o algoritmo e interrompido ao passar disso
const long long LIMITE_CHAMADAS = 50'000'000;

// Limite de profundidade: a divisao 1 + (V - 1) desce o valor de um em um,
// entao V muito grande estouraria a pilha de chamadas
const int LIMITE_PROFUNDIDADE = 10'000;

// Guarda a resposta
struct ResultadoTroco {
  int quantidadeMinima; // INFINITO se nao for possivel
  vector<int> moedasUsadas;
};

// Estatisticas da recursao, usadas nos testes
struct Estatisticas {
  long long chamadas = 0; // quantidade de chamadas recursivas
  long long divisoes = 0; // quantidade de pares (i, V - i) testados
  int profundidadeMaxima = 0;
  bool interrompido = false; // true se passou de algum dos limites
  string motivo;             // qual limite foi atingido
};

Estatisticas estatisticas;
set<int> conjuntoMoedas; // para checar o caso base "V e uma moeda" rapido

// Resolve o problema do troco por Divisao e Conquista:
// divide o valor em duas partes (i e valor - i), resolve cada parte
// recursivamente e combina ficando com a divisao de menor soma
ResultadoTroco trocoDC(int valor, int profundidade) {
  estatisticas.chamadas++;
  if (profundidade > estatisticas.profundidadeMaxima)
    estatisticas.profundidadeMaxima = profundidade;

  // Interrompe se a recursao ficou grande ou profunda demais
  if (estatisticas.chamadas > LIMITE_CHAMADAS) {
    estatisticas.interrompido = true;
    estatisticas.motivo =
        "passou de " + to_string(LIMITE_CHAMADAS) + " chamadas recursivas";
    return {INFINITO, {}};
  }
  if (profundidade > LIMITE_PROFUNDIDADE) {
    estatisticas.interrompido = true;
    estatisticas.motivo =
        "passou de " + to_string(LIMITE_PROFUNDIDADE) + " niveis de recursao";
    return {INFINITO, {}};
  }

  // Caso base 1: valor 0 precisa de 0 moedas
  if (valor == 0)
    return {0, {}};

  // Caso base 2: o valor e exatamente uma moeda
  if (conjuntoMoedas.count(valor))
    return {1, {valor}};

  ResultadoTroco melhor = {INFINITO, {}};

  // Divisao: testa todos os pares (i, valor - i), com i ate valor / 2
  // (parar na metade evita testar o mesmo par duas vezes)
  for (int i = 1; i <= valor / 2; i++) {
    estatisticas.divisoes++;

    // Conquista: resolve cada parte chamando a propria funcao.
    // Sem memorizacao, o mesmo valor e recalculado varias vezes
    ResultadoTroco esquerda = trocoDC(i, profundidade + 1);
    if (estatisticas.interrompido)
      return melhor;
    if (esquerda.quantidadeMinima == INFINITO)
      continue;

    ResultadoTroco direita = trocoDC(valor - i, profundidade + 1);
    if (estatisticas.interrompido)
      return melhor;
    if (direita.quantidadeMinima == INFINITO)
      continue;

    // Combinacao: soma as duas partes e guarda a divisao de menor total
    int total = esquerda.quantidadeMinima + direita.quantidadeMinima;
    if (total < melhor.quantidadeMinima) {
      melhor.quantidadeMinima = total;
      melhor.moedasUsadas = esquerda.moedasUsadas;
      melhor.moedasUsadas.insert(melhor.moedasUsadas.end(),
                                 direita.moedasUsadas.begin(),
                                 direita.moedasUsadas.end());
    }
  }

  return melhor;
}

// Mostra as moedas disponiveis do caso de teste
void imprimirEntrada(const vector<int> &moedas, int valor) {
  cout << "Moedas disponiveis: { ";
  for (size_t i = 0; i < moedas.size(); i++)
    cout << moedas[i] << (i + 1 < moedas.size() ? ", " : " }\n");
  cout << "Valor do troco: " << valor << "\n";
}

// Mostra o resultado de um caso de teste, agrupando moedas iguais (ex.: 3 x 2)
void imprimirResultado(const ResultadoTroco &resultado, double tempoMs) {
  if (estatisticas.interrompido) {
    cout << "Interrompido: " << estatisticas.motivo << " sem terminar.\n";
  } else if (resultado.quantidadeMinima == INFINITO) {
    cout << "Nao e possivel formar esse valor com as moedas dadas.\n";
  } else {
    cout << "Quantidade minima de moedas: " << resultado.quantidadeMinima
         << "\n";

    // Conta quantas vezes cada moeda aparece, da maior para a menor
    map<int, int, greater<int>> quantidadePorMoeda;
    for (int moeda : resultado.moedasUsadas)
      quantidadePorMoeda[moeda]++;

    cout << "Moedas usadas: ";
    for (auto [moeda, quantidade] : quantidadePorMoeda)
      cout << moeda << " x " << quantidade << "   ";
    cout << "\n";
  }

  cout << "Chamadas recursivas: " << estatisticas.chamadas << "\n";
  cout << "Divisoes testadas: " << estatisticas.divisoes << "\n";
  cout << "Profundidade maxima: " << estatisticas.profundidadeMaxima << "\n";
  cout << "Tempo de execucao: " << tempoMs << " ms\n\n";
}

int main() {
  int quantidadeMoedas;
  int numeroCaso = 1;

  // Le varios casos de teste ate receber 0 como quantidade de moedas
  while (cin >> quantidadeMoedas && quantidadeMoedas > 0) {
    vector<int> moedas(quantidadeMoedas);
    for (int &moeda : moedas)
      cin >> moeda;

    int valor;
    cin >> valor;

    cout << "===== Caso " << numeroCaso++ << " =====\n";

    if (valor < 0) {
      cout << "Valor invalido.\n\n";
      continue;
    }

    imprimirEntrada(moedas, valor);

    // Prepara o caso: zera as estatisticas e monta o conjunto de moedas
    estatisticas = Estatisticas();
    conjuntoMoedas = set<int>(moedas.begin(), moedas.end());

    // Mede o tempo apenas do algoritmo de D&C (sem contar leitura e impressao)
    auto inicio = chrono::high_resolution_clock::now();
    ResultadoTroco resultado = trocoDC(valor, 0);
    auto fim = chrono::high_resolution_clock::now();
    double tempoMs = chrono::duration<double, milli>(fim - inicio).count();

    imprimirResultado(resultado, tempoMs);
  }

  return 0;
}