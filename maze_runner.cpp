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
bool exit_found = false; // Flag para indicar se a saída foi encontrada

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
void walk(Position pos) {

    // Ajuda na impressão do labirinto
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Verifica se a posicao atual eh a saida e guarda o resultado
    if(maze[pos.row][pos.col] == 's') {
        exit_found = true;
    }

    // Marca a posicao atual como visitada
    maze[pos.row][pos.col] = '.'; 
     
    // Verifica se as posicoes vizinhas sao validas
    std::stack<Position> valid_positions;
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

    // Se houver mais de uma posição válida, cria thread para cada uma.
    // Quando sobrar apenas uma posição válida, não cria mais threads
    // (segue a thread principal)
    while(!valid_positions.empty()) {

        // Se houver apenas uma posição válida, não abre thread adicional
        if (valid_positions.size() == 1) {
            Position next_pos = valid_positions.top();
            valid_positions.pop();
            walk(next_pos); // Chama a função walk recursivamente
        }
        else {
            Position next_pos = valid_positions.top();
            valid_positions.pop();

            // Cria nova thread para seguir por esse caminho
            std::thread t([next_pos]() { 
                walk(next_pos);
            });
            t.detach();} 
    }
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

    std::thread t(walk, initial_pos);
	t.detach();

	while(!exit_found){
        print_maze();
		std::this_thread::sleep_for(std::chrono::milliseconds(45));
        system("clear");
    }
    print_maze();

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
