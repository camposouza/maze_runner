#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {

    // Abre o arquivo
    std::ifstream file(file_name);

    // Checa se nao houve erro na abertura
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return {-1, -1};
    }

    // Leh o numero de linhas e colunas
    if (!(file >> num_rows >> num_cols)) {
        std::cerr << "The first line should contain the number of rows and colums" << std::endl;
        return {-1, -1};
    }
    
    // Redimensiona a matriz 'maze'
    maze.resize(num_rows, std::vector<char>(num_cols));

    // Leh do labirinto do arquivo, caractere por caractere
    Position entrace = {-1, -1};
    for(int i = 0; i < num_rows; i++) {
        for(int j = 0; j < num_cols; j++) {
            file >> maze[i][j];
            
            // Checa se a atual celula eh a entrada
            if(maze[i][j] == 'e') entrace = {i, j};
        }
    }

    // Fecha o arquivo
    file.close();

    // Retorna a entrada
    return entrace;
}

// Função para imprimir o labirinto
void print_maze() {

    for(int i = 0; i < num_rows; i++) {
        for(int j = 0; j < num_cols; j++) {
            std::cout << maze[i][j] << " ";
        }
        std::cout << '\n';
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {

    if (row >= 0 && row < num_rows && col >= 0 && col < num_cols) {

        // A saída também deve ser considerada uma posição válida, senão
        // nunca será chamado o walk() estando nela
        if (maze[row][col] == 'x' || maze[row][col] == 's') {
            return true;
        }
    }
    return false;
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {

    // Verifica se a posicao atual eh a saida e guarda o resultado
    bool is_exit;
    if(maze[pos.row][pos.col] == 's') {
        is_exit = true;
    }
    else is_exit = false;

    // Marca a posicao atual como visitada
    maze[pos.row][pos.col] = '.';

    // Mostra o estado atual do labirinto
    print_maze();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Termina a execucao se e somente se a posicao atual eh a saida
    // (isso implica que, em algum momento, precisamos passar pela posição da saída,
    // então ela precisa ser considerada uma posição válida)
    if(is_exit) return true;    

    // Verifica se as posicoes vizinhas sao validas
    if(is_valid_position(pos.row-1, pos.col)) {
        Position cima = {pos.row-1, pos.col};
        valid_positions.push(cima);
    }
    if(is_valid_position(pos.row+1, pos.col)) {
        Position baixo = {pos.row+1, pos.col};
        valid_positions.push(baixo);
    }
    if(is_valid_position(pos.row, pos.col-1)) {
        Position esquerda = {pos.row, pos.col-1};
        valid_positions.push(esquerda);
    }
    if(is_valid_position(pos.row, pos.col+1)) {
        Position direita = {pos.row, pos.col+1};
        valid_positions.push(direita);
    }

    // Enquanto houver posicoes validas, repita
    while(!valid_positions.empty()) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();

        // Limpa o terminal
        system("clear");

        if (walk(next_pos)) return true;
    }

    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    int i = 0;
    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
