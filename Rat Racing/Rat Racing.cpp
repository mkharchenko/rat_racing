#include <iostream>
#include <cmath>
#include <vector>

const int MODULUS = 1000000007;
const int INDEXING_SHIFT = 1;

struct Transition {
    int transition_from;
    int transition_to;
};

//Умножение квадратных матриц
std::vector<std::vector<int64_t> > squareMatrixMultiplication(
    const std::vector<std::vector<int64_t> >& left_matrix,
    const std::vector<std::vector<int64_t> >& right_matrix) {
    const int matrix_size = left_matrix.size();

    if (right_matrix.size() != matrix_size) {
        throw std::invalid_argument("Incorrect matrix size.");
    }
    for (int i = 0; i < matrix_size; ++i) {
        if (left_matrix[i].size() != matrix_size ||
            right_matrix[i].size() != matrix_size) {
            throw std::invalid_argument("Matrices are not square.");
        }
    }

    std::vector<std::vector<int64_t> >
        multiplication_result(matrix_size,
            std::vector<int64_t>(matrix_size, 0));

    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            for (int z = 0; z < matrix_size; ++z) {
                multiplication_result[i][j] +=
                    left_matrix[i][z] * right_matrix[z][j];

                multiplication_result[i][j] %= MODULUS;
            }
        }
    }
    return multiplication_result;
}

//Приведение введенных путей к виду, готовому к обработке
int NormalizeTransitionIndex(const int transition_index) {
    const int normalized_transition_index =
        transition_index - INDEXING_SHIFT;
    return normalized_transition_index;
}

//Формирование матрицы смежностей
std::vector<std::vector<int64_t> > FormAdjacencyMatrix(
    const std::vector<Transition>& transitions_list,
    const int number_of_rooms) {

    std::vector<std::vector<int64_t> > adjacency_matrix(number_of_rooms,
        std::vector<int64_t>(number_of_rooms, 0));

    const int transition_list_size = transitions_list.size();

    for (int i = 0; i < transition_list_size; ++i) {
        const int transition_from =
            NormalizeTransitionIndex(transitions_list[i].transition_from);
        const int transition_to =
            NormalizeTransitionIndex(transitions_list[i].transition_to);

        if (transition_from < adjacency_matrix.size())
        {
            if (transition_to < adjacency_matrix[transition_from].size()) {
                adjacency_matrix[transition_from][transition_to] += 1;
            }
            else {
                throw std::out_of_range(
                    "transition_to parameter out of range.");
            }
        }
        else {
            throw std::out_of_range("transition_from parameter out of range.");
        }
    }

    return adjacency_matrix;
}

//Подсчитывание путей из первой вершины
//Используется матрица результатов
int64_t calculateWaysFromFirstPoint(const int number_of_rooms,
    const std::vector<std::vector<int64_t> >& ways_from_each_room) {
    int64_t ways_count = 0;
    for (int i = 0; i < number_of_rooms; ++i) {
        if (ways_from_each_room.size() > 0) {
            if (i < ways_from_each_room[0].size()) {
                ways_count += ways_from_each_room[0][i];
                ways_count %= MODULUS;
            }
            else {
                throw std::out_of_range("Ways matrix parameter out of range.");
            }
        }
        else {
            throw std::out_of_range("Ways matrix parameter out of range.");
        }
    }
    return ways_count;
}

//Вычисление матрицы результатов
int64_t CalculateNumberOfWays(const int number_of_rooms,
    const int way_length,
    const std::vector<Transition>& transitions_list) {

    std::vector<std::vector<int64_t> > matrix_to_multiply(
        number_of_rooms, std::vector<int64_t>(number_of_rooms, 0));
    std::vector<std::vector<int64_t> > matrices_product(
        number_of_rooms, std::vector<int64_t>(number_of_rooms, 0));
    const std::vector<std::vector<int64_t> > adjacency_matrix =
        FormAdjacencyMatrix(transitions_list, number_of_rooms);
    matrices_product = adjacency_matrix;

    if (way_length == 0) {
        return 1;
    }

    int matrix_power = way_length;
    int square_times = 0;
    for (int i = 1; i * 2 <= way_length; i *= 2) {
        square_times++;
    }
    const int times_to_square = square_times;

    std::vector<bool> additional_multiplication_bitmap(times_to_square, false);

    matrix_to_multiply = adjacency_matrix;

    while (matrix_power != 1) {
        if (matrix_power % 2 == 1) {
            matrix_power /= 2;
            square_times--;
            additional_multiplication_bitmap[square_times] = true;
        }
        else {
            square_times--;
            matrix_power /= 2;
        }
    }

    while (square_times < times_to_square) {
        matrices_product = squareMatrixMultiplication(
            matrix_to_multiply, matrix_to_multiply);
        matrix_power *= 2;
        if (additional_multiplication_bitmap[square_times]) {
            matrix_power++;
            matrix_to_multiply = matrices_product;
            matrices_product = squareMatrixMultiplication(
                matrix_to_multiply, adjacency_matrix);
        }
        square_times++;
        matrix_to_multiply = matrices_product;
    }

    const int64_t ways_count =
        calculateWaysFromFirstPoint(number_of_rooms, matrices_product);
    return ways_count;
}

//Выводит количество путей
void WriteWays(const int64_t ways_count,
    std::ostream& output = std::cout) {
    output << ways_count;
}

//Считывание числа комнат
int ReadNumberOfRooms(std::istream& input = std::cin) {
    int rooms_number;
    input >> rooms_number;
    return rooms_number;
}

//Считывание числа переходов между комнатами
int ReadNumberOfTransitions(std::istream& input = std::cin) {
    int transitions_number;
    input >> transitions_number;
    return transitions_number;
}

//Считывание длины искомых путей
int ReadWaysLength(std::istream& input = std::cin) {
    int way_length;
    input >> way_length;
    return way_length;
}

//Считывание всех путей в условии
//Создание матрицы смежности
std::vector<Transition> ReadWays(const int number_of_rooms,
    const int number_of_transitions,
    std::istream& input = std::cin) {
    std::vector<Transition> transitions_list(number_of_transitions);
    for (int i = 0; i < number_of_transitions; ++i) {
        Transition transition;
        input >> transition.transition_from >> transition.transition_to;
        transitions_list[i] = transition;
    }
    return transitions_list;
}

int main()
{
    const int number_of_rooms = ReadNumberOfRooms();
    const int number_of_transitions = ReadNumberOfTransitions();
    const int way_length = ReadWaysLength();
    const std::vector<Transition> transitions_list =
        ReadWays(number_of_rooms, number_of_transitions);
    const int64_t ways_count =
        CalculateNumberOfWays(number_of_rooms, way_length, transitions_list);
    WriteWays(ways_count);
    return 0;
}
