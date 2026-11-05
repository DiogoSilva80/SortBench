#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;

struct Estudante
{
    int id;
    int tipo_curso; // 0, 1, 2
    int nota; // 10 a 20
};

// Função para gerar nota com Distribuição Binomial(10, 1/3) + 10
int gerarNota()
{
    static mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    static binomial_distribution<int> d(10, 1.0 / 3.0);
    return d(gen) + 10;
}

// Função para gerar tipo de curso aleatório (0, 1 ou 2)
int gerarCurso()
{
    static mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> d(0, 2);
    return d(gen);
}

vector<Estudante> gerarDados(int n, char tipo)
{
    vector<Estudante> dados;
    dados.reserve(n);
    static mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> prob(0.0, 1.0);
    uniform_int_distribution<int> dist_multi(2, 3);

    int id_atual = 1;
    while (dados.size() < n)
    {
        int repeticoes = 1;

        // Regra dos 5%: 2 ou 3 cursos
        if (prob(gen) <= 0.05)
        {
            repeticoes = dist_multi(gen);
        }

        for (int i = 0; i < repeticoes && dados.size() < n; ++i)
        {
            dados.push_back({id_atual, gerarCurso(), gerarNota()});
        }
        id_atual++;
    }

    // Aplicar a ordem conforme o Tipo (A, B ou C)
    if (tipo == 'A')
    {
        // Já está em ordem crescente por ID devido ao loop
    }
    else if (tipo == 'B')
    {
        // Ordem decrescente de ID
        sort(dados.begin(), dados.end(), [](const Estudante& a, const Estudante& b)
        {
            if (a.id != b.id) return a.id > b.id;
            return a.tipo_curso < b.tipo_curso; // Critério secundário da Listagem 1
        });
    }
    else if (tipo == 'C')
    {
        // Ordem aleatória
        shuffle(dados.begin(), dados.end(), gen);
    }

    return dados;
}

void merge(vector<Estudante>& v, int left, int mid, int right, int modo)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<Estudante> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = v[left + i];
    for (int j = 0; j < n2; j++) R[j] = v[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        bool condition = false;
        if (modo == 1)
        {
            // Listagem 1: ID crescente, depois Tipo Curso
            if (L[i].id < R[j].id) condition = true;
            else if (L[i].id == R[j].id && L[i].tipo_curso <= R[j].tipo_curso) condition = true;
        }
        else if (modo == 3)
        {
            // Listagem 3: Nota decrescente (Estável)
            if (L[i].nota >= R[j].nota) condition = true;
        }

        if (condition)
        {
            v[k] = L[i];
            i++;
        }
        else
        {
            v[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        v[k] = L[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        v[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(vector<Estudante>& v, int left, int right, int modo)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(v, left, mid, modo);
        mergeSort(v, mid + 1, right, modo);
        merge(v, left, mid, right, modo);
    }
}

void bubbleSort(vector<Estudante>& v, int modo)
{
    int n = v.size();
    bool trocou;

    for (int i = 0; i < n - 1; i++)
    {
        trocou = false;
        for (int j = 0; j < n - i - 1; j++)
        {
            bool condition = false;

            if (modo == 1)
            {
                // Listagem 1: Crescente por ID e Tipo de Curso
                if (v[j].id > v[j + 1].id) condition = true;
                else if (v[j].id == v[j + 1].id && v[j].tipo_curso > v[j + 1].tipo_curso) condition = true;
            }
            else if (modo == 2)
            {
                // Listagem 2: Crescente por Tipo de Curso
                if (v[j].tipo_curso > v[j + 1].tipo_curso) condition = true;
            }
            else if (modo == 3)
            {
                // Listagem 3: Decrescente por Nota
                if (v[j].nota < v[j + 1].nota) condition = true;
            }

            if (condition)
            {
                swap(v[j], v[j + 1]);
                trocou = true;
            }
        }
        // Rigor: Se não houve trocas nesta passagem, o array já está ordenado
        if (!trocou) break;
    }
}

// Função auxiliar para manter a propriedade do Max-Heap / Min-Heap
void heapify(vector<Estudante>& v, int n, int i, int modo)
{
    int largest_or_smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // Função lambda para isolar a lógica de comparação
    auto compara = [&](const Estudante& a, const Estudante& b)
    {
        if (modo == 1)
        {
            // Para ordenar Crescente, precisamos de um Max-Heap (a > b)
            if (a.id != b.id) return a.id > b.id;
            return a.tipo_curso > b.tipo_curso;
        }
        else if (modo == 2)
        {
            // Max-Heap pelo Tipo de Curso
            return a.tipo_curso > b.tipo_curso;
        }
        else if (modo == 3)
        {
            // Para ordenar Decrescente, precisamos de um Min-Heap (a < b)
            return a.nota < b.nota;
        }
        return false;
    };

    if (left < n && compara(v[left], v[largest_or_smallest]))
        largest_or_smallest = left;

    if (right < n && compara(v[right], v[largest_or_smallest]))
        largest_or_smallest = right;

    if (largest_or_smallest != i)
    {
        swap(v[i], v[largest_or_smallest]);
        heapify(v, n, largest_or_smallest, modo);
    }
}

void heapSort(vector<Estudante>& v, int modo)
{
    int n = v.size();

    // 1. Construir o Heap (rearranja o array)
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify(v, n, i, modo);
    }

    // 2. Extrair os elementos do Heap um a um
    for (int i = n - 1; i > 0; i--)
    {
        // Move a raiz atual para o fim do array
        swap(v[0], v[i]);
        // Chama o heapify na árvore reduzida
        heapify(v, i, 0, modo);
    }
}

void countingSortOtimizado(vector<Estudante>& v, int modo)
{
    int n = v.size();
    if (n <= 1) return;

    vector<Estudante> output(n);

    if (modo == 2)
    {
        // Listagem 2: Crescente por Tipo de Curso (Valores 0, 1, 2)
        int count[3] = {0, 0, 0};

        for (int i = 0; i < n; i++) count[v[i].tipo_curso]++;

        // Posições acumuladas (da esquerda para a direita -> Crescente)
        for (int i = 1; i < 3; i++) count[i] += count[i - 1];

        // Iteração de trás para a frente garante a estabilidade dos dados originais
        for (int i = n - 1; i >= 0; i--)
        {
            output[count[v[i].tipo_curso] - 1] = v[i];
            count[v[i].tipo_curso]--;
        }
    }
    else if (modo == 3)
    {
        // Listagem 3: Decrescente por Nota (Valores 10 a 20)
        int count[11] = {0}; // Índices de 0 (nota 10) a 10 (nota 20)

        for (int i = 0; i < n; i++) count[v[i].nota - 10]++;

        // Posições acumuladas (da DIREITA para a ESQUERDA -> Decrescente)
        // A nota 20 (índice 10) deve ocupar as primeiras posições do array final
        for (int i = 9; i >= 0; i--) count[i] += count[i + 1];

        // Novamente, iteração inversa para estabilidade estrita
        for (int i = n - 1; i >= 0; i--)
        {
            output[count[v[i].nota - 10] - 1] = v[i];
            count[v[i].nota - 10]--;
        }
    }

    // Substituir o vetor original pelo ordenado
    v = output;
}


// Estrutura de ajuda para gerir o ciclo de testes
double medirTempoExecucao(void (*algoritmo)(vector<Estudante>&, int), vector<Estudante>& dados_originais, int modo)
{
    double soma_tempos = 0.0;


    for (int i = 0; i < 3; i++)
    {
        vector<Estudante> copia_dados = dados_originais;

        auto inicio = chrono::high_resolution_clock::now();
        algoritmo(copia_dados, modo);
        auto fim = chrono::high_resolution_clock::now();

        chrono::duration<double, milli> duracao = fim - inicio;
        soma_tempos += duracao.count();
    }

    return soma_tempos / 3.0; // Retorna a média em milissegundos
}

void imprimirCabecalho(string titulo, bool incluirOtimizacao) {
    cout << "\n=== " << titulo << " ===" << endl;
    cout << left << setw(10) << "Tamanho" << setw(6) << "Tipo"
         << setw(15) << "Merge(ms)" << setw(15) << "Heap(ms)"
         << setw(15) << "Bubble(ms)";
    if (incluirOtimizacao) cout << setw(15) << "Counting(ms)";
    cout << endl << string(70, '-') << endl;
}

int main() {
    vector<int> tamanhos = {1000, 10000, 100000, 1000000};
    vector<char> tipos = {'A', 'B', 'C'};

    // --- LISTAGEM 1 ---
    imprimirCabecalho("TESTES LISTAGEM 1 (ID + Tipo)", false);
    for (int n : tamanhos) {
        for (char tipo : tipos) {
            vector<Estudante> dados = gerarDados(n, tipo);
            double t_merge = medirTempoExecucao([](vector<Estudante>& v, int m){ mergeSort(v, 0, v.size()-1, m); }, dados, 1);
            double t_heap = medirTempoExecucao(heapSort, dados, 1);
            double t_bubble = (n <= 10000) ? medirTempoExecucao(bubbleSort, dados, 1) : -1.0;
            cout << left << setw(10) << n << setw(6) << tipo << fixed << setprecision(3) << setw(15) << t_merge << setw(15) << t_heap;
            if (t_bubble != -1.0) cout << setw(15) << t_bubble << endl; else cout << setw(15) << "N/A" << endl;
        }
    }

    // --- LISTAGEM 2 ---
    imprimirCabecalho("TESTES LISTAGEM 2 (Tipo de Curso)", true);
    for (int n : tamanhos) {
        for (char tipo : tipos) {
            vector<Estudante> dados = gerarDados(n, tipo);
            double t_merge = medirTempoExecucao([](vector<Estudante>& v, int m){ mergeSort(v, 0, v.size()-1, m); }, dados, 2);
            double t_heap = medirTempoExecucao(heapSort, dados, 2);
            double t_bubble = (n <= 10000) ? medirTempoExecucao(bubbleSort, dados, 2) : -1.0;
            double t_count = medirTempoExecucao(countingSortOtimizado, dados, 2);
            cout << left << setw(10) << n << setw(6) << tipo << fixed << setprecision(3) << setw(15) << t_merge << setw(15) << t_heap;
            if (t_bubble != -1.0) cout << setw(15) << t_bubble; else cout << setw(15) << "N/A";
            cout << setw(15) << t_count << endl;
        }
    }

    // --- LISTAGEM 3 ---
    imprimirCabecalho("TESTES LISTAGEM 3 (Nota Decrescente)", true);
    for (int n : tamanhos) {
        for (char tipo : tipos) {
            vector<Estudante> dados = gerarDados(n, tipo);
            double t_merge = medirTempoExecucao([](vector<Estudante>& v, int m){ mergeSort(v, 0, v.size()-1, m); }, dados, 3);
            double t_heap = medirTempoExecucao(heapSort, dados, 3);
            double t_bubble = (n <= 10000) ? medirTempoExecucao(bubbleSort, dados, 3) : -1.0;
            double t_count = medirTempoExecucao(countingSortOtimizado, dados, 3);
            cout << left << setw(10) << n << setw(6) << tipo << fixed << setprecision(3) << setw(15) << t_merge << setw(15) << t_heap;
            if (t_bubble != -1.0) cout << setw(15) << t_bubble; else cout << setw(15) << "N/A";
            cout << setw(15) << t_count << endl;
        }
    }

    return 0;
}
