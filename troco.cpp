#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

const int INFINITO = 1e9;

// Guarda a resposta
struct ResultadoTroco {
  int quantidadeMinima;      // -1 se nao for possivel
  vector<int> moedasUsadas;
};

// Resolve o problema do troco com bottom-up
ResultadoTroco calcularTroco(const vector<int>& moedas, int valor) {
  // minMoedas[i] = menor numero de moedas para formar o valor i
  vector<int> minMoedas(valor + 1, INFINITO);

  // ultimaMoeda[i] = moeda usada por ultimo para formar i (para reconstruir a solucao)
  vector<int> ultimaMoeda(valor + 1, -1);

  minMoedas[0] = 0;  // caso base: valor 0 precisa de 0 moedas

  // Preenche a tabela do menor valor para o maior
  for (int atual = 1; atual <= valor; atual++) {
    for (int moeda : moedas) {
      // Testa usar "moeda" como a ultima: reutiliza o resultado ja
      // armazenado de (atual - moeda), sem recalcular
      if (moeda <= atual && minMoedas[atual - moeda] != INFINITO &&
          minMoedas[atual - moeda] + 1 < minMoedas[atual]) {
        minMoedas[atual] = minMoedas[atual - moeda] + 1;
        ultimaMoeda[atual] = moeda;
      }
    }
  }

  ResultadoTroco resultado;

  // Se o valor final continua INFINITO, nao existe combinacao possivel
  if (minMoedas[valor] == INFINITO) {
    resultado.quantidadeMinima = -1;
    return resultado;
  }

  resultado.quantidadeMinima = minMoedas[valor];

  // Reconstroi a solucao voltando do valor final ate 0,
  // seguindo a moeda escolhida em cada passo
  for (int restante = valor; restante > 0; restante -= ultimaMoeda[restante])
    resultado.moedasUsadas.push_back(ultimaMoeda[restante]);

  return resultado;
}

// Mostra as moedas disponiveis do caso de teste
void imprimirEntrada(const vector<int>& moedas, int valor) {
  cout << "Moedas disponiveis: { ";
  for (size_t i = 0; i < moedas.size(); i++)
    cout << moedas[i] << (i + 1 < moedas.size() ? ", " : " }\n");
  cout << "Valor do troco: " << valor << "\n";
}

// Mostra o resultado de um caso de teste, agrupando moedas iguais (ex.: 22 x 12)
void imprimirResultado(const ResultadoTroco& resultado, double tempoMs) {
  if (resultado.quantidadeMinima == -1) {
    cout << "Nao e possivel formar esse valor com as moedas dadas.\n";
  } else {
    cout << "Quantidade minima de moedas: " << resultado.quantidadeMinima << "\n";

    // Conta quantas vezes cada moeda aparece, da maior para a menor
    map<int, int, greater<int>> quantidadePorMoeda;
    for (int moeda : resultado.moedasUsadas) quantidadePorMoeda[moeda]++;

    cout << "Moedas usadas: ";
    for (auto [moeda, quantidade] : quantidadePorMoeda)
      cout << moeda << " x " << quantidade << "   ";
    cout << "\n";
  }
  cout << "Tempo de execucao: " << tempoMs << " ms\n\n";
}

int main() {
  int quantidadeMoedas;
  int numeroCaso = 1;

  // Le varios casos de teste ate receber 0 como quantidade de moedas
  while (cin >> quantidadeMoedas && quantidadeMoedas > 0) {
    vector<int> moedas(quantidadeMoedas);
    for (int& moeda : moedas) cin >> moeda;

    int valor;
    cin >> valor;

    cout << "===== Caso " << numeroCaso++ << " =====\n";

    if (valor < 0) {
      cout << "Valor invalido.\n\n";
      continue;
    }

    imprimirEntrada(moedas, valor);

    // Mede o tempo apenas do algoritmo de PD (sem contar leitura e impressao)
    auto inicio = chrono::high_resolution_clock::now();
    ResultadoTroco resultado = calcularTroco(moedas, valor);
    auto fim = chrono::high_resolution_clock::now();
    double tempoMs = chrono::duration<double, milli>(fim - inicio).count();

    imprimirResultado(resultado, tempoMs);
  }

  return 0;
}