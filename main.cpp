#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <iterator>

using namespace std;

/*
    Registro de tamanho fixo para armazenar uma palavra,
    de até 29 caracteres ASCII mais o terminador.
*/
struct Registro {
    char palavra[30];
};

class ListaPalavras {

private:

    vector<Registro> palavras;

    /*
        Converte uma sequencia UTF-8 para seu equivalente
        sem acentuacao e em maiusculo.

        Exemplos:
        á -> A
        ç -> C
        õ -> O
        é -> E

        Caracteres ASCII tambem sao convertidos para maiusculo.
    */
    string normalizaTexto(string texto) {

        string resultado;

        for (size_t i = 0; i < texto.size(); i++) {

            unsigned char c = texto[i];

            // Caracteres ASCII
            if (c < 128) {

                resultado += (char)toupper(c);
            }

            // Caracteres UTF-8 de dois bytes
            else if ((c & 0xE0) == 0xC0 && i + 1 < texto.size()) {

                unsigned char c2 = texto[i + 1];

                // Verifica se a sequencia UTF-8 e valida
                if ((c2 & 0xC0) != 0x80) {
                    resultado += ' ';
                    continue;
                }

                unsigned int codigo =
                    ((c & 0x1F) << 6) | (c2 & 0x3F);

                i++;

                switch (codigo) {

                    // A
                    case 0x00C0: case 0x00C1:
                    case 0x00C2: case 0x00C3:
                    case 0x00C4: case 0x00C5:
                    case 0x00E0: case 0x00E1:
                    case 0x00E2: case 0x00E3:
                    case 0x00E4: case 0x00E5:
                        resultado += 'A';
                        break;

                    // C
                    case 0x00C7: case 0x00E7:
                        resultado += 'C';
                        break;

                    // E
                    case 0x00C8: case 0x00C9:
                    case 0x00CA: case 0x00CB:
                    case 0x00E8: case 0x00E9:
                    case 0x00EA: case 0x00EB:
                        resultado += 'E';
                        break;

                    // I
                    case 0x00CC: case 0x00CD:
                    case 0x00CE: case 0x00CF:
                    case 0x00EC: case 0x00ED:
                    case 0x00EE: case 0x00EF:
                        resultado += 'I';
                        break;

                    // N
                    case 0x00D1: case 0x00F1:
                        resultado += 'N';
                        break;

                    // O
                    case 0x00D2: case 0x00D3:
                    case 0x00D4: case 0x00D5:
                    case 0x00D6:
                    case 0x00F2: case 0x00F3:
                    case 0x00F4: case 0x00F5:
                    case 0x00F6:
                        resultado += 'O';
                        break;

                    // U
                    case 0x00D9: case 0x00DA:
                    case 0x00DB: case 0x00DC:
                    case 0x00F9: case 0x00FA:
                    case 0x00FB: case 0x00FC:
                        resultado += 'U';
                        break;

                    // Y
                    case 0x00DD: case 0x00FD:
                    case 0x00FF:
                        resultado += 'Y';
                        break;

                    // Caracteres nao reconhecidos
                    default:
                        resultado += ' ';
                        break;
                }
            }

            // Outros caracteres UTF-8 sao ignorados como separadores
            else {
                resultado += ' ';
            }
        }

        return resultado;
    }


    // Verifica se um caractere pertence a uma palavra.
    bool ehCaracterePalavra(char c) {

        return isalnum((unsigned char)c);
    }


    // Adiciona uma palavra a lista.
    void adiciona(string palavra) {

        if (palavra.empty()) {
            return;
        }

        Registro r;

        strncpy(r.palavra, palavra.c_str(), 29);
        r.palavra[29] = '\0';

        palavras.push_back(r);
    }


public:

    /*
        Carrega palavras de um arquivo texto.

        1. Le o conteudo do arquivo.
        2. Remove os acentos.
        3. Converte tudo para maiusculo.
        4. Separa as palavras.
        5. Armazena os registros.
    */
    void loadFromFile(string nomeArquivo) {

        ifstream fin(nomeArquivo);

        if (!fin.is_open()) {
            cout << "Erro ao abrir o arquivo!" << endl;
            return;
        }

        string texto(
            (istreambuf_iterator<char>(fin)),
            istreambuf_iterator<char>()
        );

        fin.close();

        // Normaliza todo o texto antes de extrair as palavras.
        texto = normalizaTexto(texto);

        string palavra;

        for (size_t i = 0; i < texto.size(); i++) {

            char c = texto[i];

            if (ehCaracterePalavra(c)) {

                palavra += c;
            }
            else {

                if (!palavra.empty()) {
                    adiciona(palavra);
                    palavra.clear();
                }
            }
        }

        // Processa a ultima palavra.
        if (!palavra.empty()) {
            adiciona(palavra);
        }
    }


    /*
        Ordena as palavras em ordem alfabetica crescente.
        Este é um exemplo de processo de ordenação que deve ser usado na fase
        de criação de runs da ordenação por intercalação.
    */
    /*
    void sort() {

        std::sort(
            palavras.begin(),
            palavras.end(),
            [](const Registro& a, const Registro& b) {

                return strcmp(a.palavra, b.palavra) < 0;
            }
        );
    }
    */


    /*
        Salva os registros em um arquivo binario.
    */
    void saveToFile(string nomeArquivo) {

        ofstream fout(nomeArquivo, ios::binary);

        if (!fout.is_open()) {
            cout << "Erro ao criar o arquivo!" << endl;
            return;
        }

        fout.write(
            (char *)palavras.data(),
            palavras.size() * sizeof(Registro)
        );

        fout.close();
    }


    // Retorna a quantidade de palavras armazenadas.
    long long size() {

        return palavras.size();
    }


    // Retorna o registro na posicao i.
    Registro get(long long i) {

        Registro r = {};

        if (i < 0 || i >= (long long)palavras.size()) {
            return r;
        }

        return palavras[i];
    }

};


// Classe que vai ordenar as palavras

class IntercalacaoTest {
private:

    /*
     * Funcao: mostraArquivo
     *
     * Finalidade:
     * Exibir as palavras armazenadas em um arquivo binario.
     *
     * Parametro:
     * nomeArquivo - nome do arquivo que sera exibido.
     *
     * A funcao e privada porque sera utilizada pelos demais
     * metodos da classe para mostrar as runs geradas.
     */
    void mostraArquivo(const char *nomeArquivo) {
        ifstream fin(nomeArquivo, ios::binary);

        if (!fin) {
            throw runtime_error("Erro ao abrir arquivo para exibicao.");
        }

        Registro valor;
        bool primeiro = true;

        while (fin.read((char *)&valor, sizeof(Registro))) {
            if (!primeiro) {
                cout << ", ";
            }

            cout << valor.palavra;
            primeiro = false;
        }

        if (!fin.eof()) {
            throw runtime_error("Erro durante a leitura do arquivo.");
        }

        cout << '\n';
    }

    /*
     * Funcao: intercala
     *
     * Finalidade:
     * Intercalar dois arquivos ordenados, produzindo um terceiro
     * arquivo tambem ordenado.
     *
     * Parametros:
     * run1    - primeiro arquivo ordenado.
     * run2    - segundo arquivo ordenado.
     * run_out - arquivo que recebera o resultado.
     *
     * Depois de gerar o arquivo de saida, a funcao o exibe
     * imediatamente na tela.
     */
    void intercala(const char *run1, const char *run2, const char *run_out) {
        ifstream fin1(run1, ios::binary);
        ifstream fin2(run2, ios::binary);
        ofstream fout(run_out, ios::binary);

        if (!fin1 || !fin2 || !fout) {
            throw runtime_error("Erro ao abrir arquivos para intercalacao.");
        }

        Registro a, b;

        bool temA = bool(fin1.read((char *)&a, sizeof(Registro)));
        bool temB = bool(fin2.read((char *)&b, sizeof(Registro)));

        while (temA && temB) {
            if (strcmp(a.palavra, b.palavra) <= 0) {
                fout.write((char *)&a, sizeof(Registro));
                temA = bool(fin1.read((char *)&a, sizeof(Registro)));
            } else {
                fout.write((char *)&b, sizeof(Registro));
                temB = bool(fin2.read((char *)&b, sizeof(Registro)));
            }
        }

        while (temA) {
            fout.write((char *)&a, sizeof(Registro));
            temA = bool(fin1.read((char *)&a, sizeof(Registro)));
        }

        while (temB) {
            fout.write((char *)&b, sizeof(Registro));
            temB = bool(fin2.read((char *)&b, sizeof(Registro)));
        }

        if (!fout) {
            throw runtime_error("Erro ao gravar arquivo intercalado.");
        }

        fout.close();

        cout << "\nArquivo temporario gerado: " << run_out << '\n';
        cout << "Conteudo: ";
        mostraArquivo(run_out);
    }

    /*
     * Funcao: identificador
     *
     * Finalidade:
     * Extrair a identificacao de uma run a partir do nome.
     *
     * Exemplos:
     * run_1.tmp       -> 1
     * run_1_2.tmp     -> 1_2
     * run_1_2_3.tmp   -> 1_2_3
     */
    string identificador(const string &nome) {
        size_t inicio = nome.find("run_");
        size_t fim = nome.rfind(".tmp");

        if (inicio == string::npos ||
            fim == string::npos ||
            fim <= inicio + 4) {
            throw runtime_error("Nome de arquivo temporario invalido.");
        }

        return nome.substr(inicio + 4, fim - (inicio + 4));
    }

public:

    /*
     * Funcao: ordenacao
     *
     * Finalidade:
     * Ordenar externamente um arquivo binario.
     *
     * Parametros:
     * in                - arquivo de entrada.
     * out               - arquivo final ordenado.
     * capacidadeMemoria - quantidade maxima de palavras
     *                     armazenadas em cada run inicial.
     *
     * Etapas:
     * 1. Ler blocos limitados pela capacidade de memoria.
     * 2. Ordenar cada bloco e grava-lo em uma run.
     * 3. Exibir cada run inicial apos sua criacao.
     * 4. Intercalar pares de runs e exibir os resultados.
     * 5. Copiar a run final para o arquivo de saida.
     */
    void ordenacao(const char *in, const char *out, int capacidadeMemoria) {
        if (capacidadeMemoria <= 0) {
            throw invalid_argument("A capacidade da memoria deve ser positiva.");
        }

        ifstream fin(in, ios::binary);

        if (!fin) {
            throw runtime_error("Erro ao abrir arquivo de entrada.");
        }

        vector<string> runs;
        vector<Registro> bloco;

        bloco.reserve(capacidadeMemoria);

        Registro valor;
        int indice = 1;

        cout << "\nFASE 1: ";
        cout << "Criacao das runs iniciais\n";

        // Le e ordena blocos de ate capacidadeMemoria palavras.
        while (fin.read((char *)&valor, sizeof(Registro))) {
            bloco.push_back(valor);

            if ((int)bloco.size() == capacidadeMemoria) {
                sort(bloco.begin(), bloco.end(),
                    [](const Registro& a, const Registro& b) {
                        return strcmp(a.palavra, b.palavra) < 0;
                    });

                string nome = "run_" + to_string(indice++) + ".tmp";
                ofstream fout(nome, ios::binary);

                if (!fout) {
                    throw runtime_error("Erro ao criar run inicial.");
                }

                for (size_t i = 0; i < bloco.size(); i++) {
                    fout.write((char *)&bloco[i], sizeof(Registro));
                }

                if (!fout) {
                    throw runtime_error("Erro ao gravar run inicial.");
                }

                fout.close();
                runs.push_back(nome);

                cout << "\nRun inicial gerada: " << nome << '\n';
                cout << "Conteudo: ";
                mostraArquivo(nome.c_str());

                bloco.clear();
            }
        }

        if (!fin.eof()) {
            throw runtime_error("Erro durante a leitura do arquivo de entrada.");
        }

        fin.close();

        // Grava o ultimo bloco, caso tenha sobrado algum elemento.
        if (!bloco.empty()) {
            sort(bloco.begin(), bloco.end(),
                [](const Registro& a, const Registro& b) {
                    return strcmp(a.palavra, b.palavra) < 0;
                });

            string nome = "run_" + to_string(indice++) + ".tmp";
            ofstream fout(nome, ios::binary);

            if (!fout) {
                throw runtime_error("Erro ao criar ultima run inicial.");
            }

            for (size_t i = 0; i < bloco.size(); i++) {
                fout.write((char *)&bloco[i], sizeof(Registro));
            }

            if (!fout) {
                throw runtime_error("Erro ao gravar ultima run inicial.");
            }

            fout.close();
            runs.push_back(nome);

            cout << "\nRun inicial gerada: " << nome << '\n';
            cout << "Conteudo: ";
            mostraArquivo(nome.c_str());
        }

        // Trata arquivo de entrada vazio.
        if (runs.empty()) {
            ofstream fout(out, ios::binary);

            if (!fout) {
                throw runtime_error("Erro ao criar arquivo de saida.");
            }

            fout.close();
            cout << "\nArquivo de saida vazio: " << out << '\n';
            return;
        }

        cout << "\nFASE 2: ";
        cout << "Intercalacao das runs\n";

        // Intercala os arquivos aos pares ate restar uma run.
        while (runs.size() > 1) {
            vector<string> proximas;

            for (size_t i = 0; i < runs.size(); i += 2) {
                if (i + 1 < runs.size()) {
                    string id1 = identificador(runs[i]);
                    string id2 = identificador(runs[i + 1]);
                    string nome = "run_" + id1 + "_" + id2 + ".tmp";

                    cout << "\nIntercalando " << runs[i]
                         << " e " << runs[i + 1] << '\n';

                    intercala(runs[i].c_str(), runs[i + 1].c_str(), nome.c_str());
                    proximas.push_back(nome);
                } else {
                    // Preserva a run que ficou sem par.
                    proximas.push_back(runs[i]);
                }
            }

            runs = proximas;
        }

        // Copia a run final para o arquivo de saida.
        ifstream origem(runs[0], ios::binary);
        ofstream destino(out, ios::binary);

        if (!origem || !destino) {
            throw runtime_error("Erro ao abrir arquivos para copia final.");
        }

        char buffer[4096];

        while (origem.read(buffer, sizeof(buffer))) {
            destino.write(buffer, origem.gcount());
        }

        if (origem.gcount() > 0) {
            destino.write(buffer, origem.gcount());
        }

        if (!origem.eof() || !destino) {
            throw runtime_error("Erro durante a copia para o arquivo de saida.");
        }

        destino.close();
        origem.close();

        cout << "\nRESULTADO FINAL\n";
        cout << "Arquivo ordenado: " << out << '\n';
        cout << "Conteudo: ";
        mostraArquivo(out);
    }
};

int main() {
    try {
        ListaPalavras lista;
        IntercalacaoTest teste;

        // Primeiro carrega o texto e cria o arquivo binario palavras.bin.
        lista.loadFromFile("o-cortico.txt");
        lista.saveToFile("palavras.bin");

        cout << "Quantidade de palavras: " << lista.size() << endl;

        int capacidadeMemoria = 5000;

        cout << "\nCapacidade da memoria: "
             << capacidadeMemoria
             << " palavras por run.\n";

        // Usa palavras.bin como arquivo binario de entrada da ordenacao.
        teste.ordenacao("palavras.bin", "ordenado.bin", capacidadeMemoria);

        cout << "\nOrdenacao concluida.\n";
        cout << "Arquivo final salvo em ordenado.bin.\n";

    } catch (const exception &e) {
        cerr << "Erro: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

