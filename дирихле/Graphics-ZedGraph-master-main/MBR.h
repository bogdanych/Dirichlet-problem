#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>   
#include <algorithm>

class SORSolver {
private:
    static const double PI;

    // Параметры области
    const double a, b, c, d;

    // Параметры сетки
    int n, m;
    double h, k, inv_h2, inv_k2, diag;

    // Параметры МВР
    double w;
    int Nmax;
    double eps;

    // Массивы
    double** v;
    double** f;
    double** u_exact;  // точное решение во всех узлах

    // Результаты решения
    int total_iterations;
    double final_residual;
    double final_error;
    double initial_residual;
    double error_norm_max;
    double residual_norm_euclid;
    double residual_norm_max;
    int error_node_i, error_node_j;
    long long solve_time_ms;

    double exact(double x, double y) const {
        return exp(1.0 - x * x - y * y);
    }

    double f_source(double x, double y) const {
        double u = exact(x, y);
        return 4.0 * (1.0 - x * x - y * y) * u;
    }

    double boundary(double x, double y) const {
        return exact(x, y);
    }

    double** alloc(int nx, int ny) {
        double** arr = new double* [nx];
        for (int i = 0; i < nx; i++) {
            arr[i] = new double[ny];
            for (int j = 0; j < ny; j++) arr[i][j] = 0.0;
        }
        return arr;
    }

    void free_arr(double**& arr, int nx) {
        if (arr) {
            for (int i = 0; i < nx; i++) delete[] arr[i];
            delete[] arr;
            arr = nullptr;
        }
    }

    // Проверка, находится ли узел (i,j) в расчётной области G
    bool is_in_domain(int i, int j) const {
        if (j == 0 && i <= n / 2) return false;         // левая нижняя граница
        if (i == n / 2 && j <= m / 2) return false;     // нижняя горизонтальная граница
        if (i > n / 2 && j == m / 2) return false;      // вертикальная граница справа
        if (i == n && j >= m / 2) return false;         // правая граница
        return true;
    }

    // Является ли узел граничным в области G
    bool is_boundary(int i, int j) const {
        if (i == 0) return true;                        // левая граница
        if (j == m) return true;                        // верхняя граница
        if (j == 0 && i < n / 2) return true;          // нижняя граница (левая часть)
        if (i == n / 2 && j <= m / 2) return true;     // внутренняя горизонталь
        if (j == m / 2 && i >= n / 2) return true;     // внутренняя вертикаль
        if (i == n && j >= m / 2) return true;         // правая граница (верхняя часть)
        return false;
    }

    // Вычисление невязки в узле (i,j)
    double residual_at(int i, int j) const {
        double res = f[i][j] - (
            (v[i - 1][j] + v[i + 1][j]) * inv_h2 +
            (v[i][j - 1] + v[i][j + 1]) * inv_k2 -
            diag * v[i][j]
            );
        return res;
    }

    double norm_max_2d(double** arr, int ni, int nj, int i_start, int j_start) const {
        double mx = 0.0;
        for (int i = i_start; i < ni; i++) {
            for (int j = j_start; j < nj; j++) {
                if (is_in_domain(i, j)) {
                    mx = std::max(mx, fabs(arr[i][j]));
                }
            }
        }
        return mx;
    }

    double norm_euclid_2d(double** arr, int ni, int nj, int i_start, int j_start) const {
        double sum = 0.0;
        for (int i = i_start; i < ni; i++) {
            for (int j = j_start; j < nj; j++) {
                if (is_in_domain(i, j)) {
                    sum += arr[i][j] * arr[i][j];
                }
            }
        }
        return sqrt(sum);
    }

public:
    SORSolver(int grid_n = 10, int grid_m = 10, int max_iter = 100000, double tolerance = 1e-10)
        : a(-1.0), b(1.0), c(-1.0), d(1.0),
        n(grid_n), m(grid_m), Nmax(max_iter), eps(tolerance),
        v(nullptr), f(nullptr), u_exact(nullptr),
        total_iterations(0), final_residual(0.0), final_error(0.0),
        initial_residual(0.0), error_node_i(0), error_node_j(0),
        solve_time_ms(0) {
        w = 2.0 / (1.0 + sin(PI / n));
    }

    ~SORSolver() {
        cleanup();
    }

    void init() {
        h = (b - a) / n;
        k = (d - c) / m;
        inv_h2 = 1.0 / (h * h);
        inv_k2 = 1.0 / (k * k);
        diag = 2.0 * (inv_h2 + inv_k2);

        v = alloc(n + 1, m + 1);
        f = alloc(n + 1, m + 1);
        u_exact = alloc(n + 1, m + 1);

        double x, y;

        for (int i = 0; i <= n; i++) {
            x = a + i * h;
            for (int j = 0; j <= m; j++) {
                y = c + j * k;
                u_exact[i][j] = exact(x, y);

                if (is_in_domain(i, j) && !is_boundary(i, j)) {
                    f[i][j] = f_source(x, y);
                    v[i][j] = 0.0;  // нулевое начальное приближение
                }

                if (is_boundary(i, j)) {
                    v[i][j] = boundary(x, y);
                }
            }
        }
    }

    void solve() {
        auto start = std::chrono::high_resolution_clock::now();

        int iter = 0;
        double eps_max;

        // Вычисляем начальную невязку
        double** residual = alloc(n + 1, m + 1);
        for (int i = 1; i < n; i++) {
            int start_j = 1;
            if (i >= n / 2) start_j = m / 2 + 1;
            for (int j = start_j; j < m; j++) {
                if (is_in_domain(i, j) && !is_boundary(i, j)) {
                    residual[i][j] = residual_at(i, j);
                }
            }
        }
        initial_residual = norm_max_2d(residual, n, m, 1, 1);
        free_arr(residual, n + 1);

        const double w1 = 1.0 - w;
        const double wd = w / diag;

        while (iter < Nmax) {
            eps_max = 0.0;

            for (int i = 1; i < n; i++) {
                int start_j = 1;
                if (i >= n / 2) start_j = m / 2 + 1;
                for (int j = start_j; j < m; j++) {
                    if (!is_in_domain(i, j) || is_boundary(i, j)) continue;

                    double v_old = v[i][j];
                    double v_new = w1 * v_old + wd * (
                        (v[i - 1][j] + v[i + 1][j]) * inv_h2 +
                        (v[i][j - 1] + v[i][j + 1]) * inv_k2 +
                        f[i][j]
                        );
                    v[i][j] = v_new;
                    eps_max = std::max(eps_max, fabs(v_new - v_old));
                }
            }

            iter++;
            if (eps_max < eps) break;
        }

        total_iterations = iter;
        final_residual = eps_max;

        // Вычисляем финальную невязку
        residual = alloc(n + 1, m + 1);
        for (int i = 1; i < n; i++) {
            int start_j = 1;
            if (i >= n / 2) start_j = m / 2 + 1;
            for (int j = start_j; j < m; j++) {
                if (is_in_domain(i, j) && !is_boundary(i, j)) {
                    residual[i][j] = residual_at(i, j);
                }
            }
        }
        residual_norm_max = norm_max_2d(residual, n, m, 1, 1);
        residual_norm_euclid = norm_euclid_2d(residual, n, m, 1, 1);
        free_arr(residual, n + 1);

        // Вычисляем погрешность
        double** error = alloc(n + 1, m + 1);
        error_node_i = 0;
        error_node_j = 0;
        error_norm_max = 0.0;
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                if (is_in_domain(i, j)) {
                    error[i][j] = fabs(u_exact[i][j] - v[i][j]);
                    if (error[i][j] > error_norm_max) {
                        error_norm_max = error[i][j];
                        error_node_i = i;
                        error_node_j = j;
                    }
                }
            }
        }
        final_error = error_norm_max;
        free_arr(error, n + 1);

        auto end = std::chrono::high_resolution_clock::now();
        solve_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    // Геттеры для справки
    int getN() const { return n; }
    int getM() const { return m; }
    double getH() const { return h; }
    double getK() const { return k; }
    double getW() const { return w; }
    int getTotalIterations() const { return total_iterations; }
    double getFinalResidual() const { return final_residual; }
    double getFinalError() const { return final_error; }
    double getInitialResidual() const { return initial_residual; }
    double getResidualNormEuclid() const { return residual_norm_euclid; }
    double getResidualNormMax() const { return residual_norm_max; }
    int getErrorNodeI() const { return error_node_i; }
    int getErrorNodeJ() const { return error_node_j; }
    long long getSolveTime() const { return solve_time_ms; }
    double getErrorNodeX() const { return a + error_node_i * h; }
    double getErrorNodeY() const { return c + error_node_j * k; }
    int getNmax() const { return Nmax; }
    double getEps() const { return eps; }

    double getSolution(int i, int j) const { return v[i][j]; }
    double getExactSolution(int i, int j) const { return u_exact[i][j]; }
    double getError(int i, int j) const { return fabs(u_exact[i][j] - v[i][j]); }

    void setGridSize(int new_n, int new_m) {
        n = new_n;
        m = new_m;
        w = 2.0 / (1.0 + sin(PI / n));
    }

    void setMaxIterations(int max_iter) { Nmax = max_iter; }
    void setTolerance(double tolerance) { eps = tolerance; }

    void cleanup() {
        if (v) {
            for (int i = 0; i <= n; i++) {
                delete[] v[i];
                if (f && f[i]) delete[] f[i];
                if (u_exact && u_exact[i]) delete[] u_exact[i];
            }
            delete[] v; v = nullptr;
            delete[] f; f = nullptr;
            delete[] u_exact; u_exact = nullptr;
        }
    }

    // Вывод справки в соответствии с шаблоном из задания
    void printReference() const {
        double x_err = a + error_node_i * h;
        double y_err = c + error_node_j * k;

        std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
        std::cout << "                С П Р А В К А  Д Л Я  Т Е С Т О В О Й  З А Д А Ч И" << std::endl;
        std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
        std::cout << std::endl;
        std::cout << "Для решения тестовой задачи использована сетка-основа" << std::endl;
        std::cout << "с числом разбиений по x n = " << n << " и числом разбиений по y m = " << m << "," << std::endl;
        std::cout << std::endl;
        std::cout << "метод: Метод верхней релаксации (МВР)," << std::endl;
        std::cout << "параметры: ω = " << std::fixed << std::setprecision(6) << w;
        std::cout << " (оптимальное значение для сетки: ω_opt = 2/(1+sin(π/n)))" << std::endl;
        std::cout << std::endl;
        std::cout << "критерии остановки:" << std::endl;
        std::cout << "  по точности ε_мет = " << std::scientific << eps << std::endl;
        std::cout << "  по числу итераций N_max = " << Nmax << std::endl;
        std::cout << std::endl;
        std::cout << "На решение схемы (СЛАУ) затрачено итераций N = " << total_iterations << std::endl;
        std::cout << "и достигнута точность итерационного метода ε(N) = ";
        std::cout << std::scientific << final_residual << std::endl;
        std::cout << std::endl;
        std::cout << "Схема (СЛАУ) решена с невязкой:" << std::endl;
        std::cout << "  ||R(N)||_max = " << std::scientific << residual_norm_max;
        std::cout << " (норма max)" << std::endl;
        std::cout << "  ||R(N)||_2   = " << std::scientific << residual_norm_euclid;
        std::cout << " (евклидова норма)" << std::endl;
        std::cout << std::endl;
        std::cout << "Невязка СЛАУ на начальном приближении:" << std::endl;
        std::cout << "  ||R(0)||_max = " << std::scientific << initial_residual;
        std::cout << " (норма max)" << std::endl;
        std::cout << std::endl;
        std::cout << "Тестовая задача должна быть решена с погрешностью не более ε = 0.5·10⁻⁶;" << std::endl;
        std::cout << "задача решена с погрешностью ε₁ = " << std::scientific << final_error << std::endl;
        std::cout << std::endl;
        std::cout << "Максимальное отклонение точного и численного решений" << std::endl;
        std::cout << "наблюдается в узле:" << std::endl;
        std::cout << "  x = " << std::fixed << std::setprecision(6) << x_err << std::endl;
        std::cout << "  y = " << std::fixed << std::setprecision(6) << y_err << std::endl;
        std::cout << "  индекс узла: i = " << error_node_i << ", j = " << error_node_j << std::endl;
        std::cout << std::endl;
        std::cout << "В качестве начального приближения использовано:" << std::endl;
        std::cout << "  нулевое начальное приближение" << std::endl;
        std::cout << std::endl;
        std::cout << "Время вычислений: " << solve_time_ms << " мс" << std::endl;
        std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
    }




    void printSolution() const noexcept {
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\nРешение:\n";
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                std::cout << std::setw(10) << v[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

const double SORSolver::PI = 3.14159265358979323846;