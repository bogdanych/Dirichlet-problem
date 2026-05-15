#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>   
#include <algorithm>
#include <clocale>
#include <iomanip>


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
    double** u_exact; 

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
        return -4.0 * exp(1.0 - x * x - y * y) * (x * x + y * y - 1.0);
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


    double residual_at(int i, int j) const {
        double A_v = (v[i - 1][j] + v[i + 1][j]) * inv_h2
            + (v[i][j - 1] + v[i][j + 1]) * inv_k2
            - diag * v[i][j];   

        return f[i][j] - A_v;  
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
        //cleanup();

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
                u_exact[i][j] = 0;

                if (is_in_domain(i, j))
                    u_exact[i][j] = exact(x, y);

                if (is_in_domain(i, j) && !is_boundary(i, j)) {
                    f[i][j] = f_source(x, y);
                    //v[i][j] = 0.0;  
                }

                if (is_boundary(i, j)) {
                    v[i][j] = boundary(x, y);
                }
            }
        }

        for (int i = 0; i <= n; i++) {
            x = a + i * h;
            int start = 0;
            if (i >= n / 2)
                start = (m + 1) / 2;

            double ly = c + start * k;
            double ry = c + m * k;

            for (int j = start + 1; j <= m; j++) {
                y = c + j * k;
                if (is_boundary(i, j))
                    continue;
                double t = (y - ly) / (ry - ly);
                v[i][j] = (1 - t) * v[i][start] + t * v[i][m];
            }

        }

    }

    void solve() {
        auto start = std::chrono::high_resolution_clock::now();

        int iter = 0;
        double eps_max;

        // начальное приближение
        std::ofstream file_v0("v_initial.txt");
        file_v0 << std::scientific << std::setprecision(10);
        file_v0 << "# x y v_initial\n";
        for (int i = 0; i <= n; i++) {
            double x = a + i * h;
            for (int j = 0; j <= m; j++) {
                if (is_in_domain(i, j)) {
                    double y = c + j * k;
                    file_v0 << x << " " << y << " " << v[i][j] << "\n";
                }
            }
        }
        file_v0.close();

        // начальная невязка
        double** residual = alloc(n + 1, m + 1);
        double max_residual_initial = 0.0;
        double sum_sq_residual_initial = 0.0;

        for (int i = 1; i <= n; i++) {
            int start_j = 1;
            if (i >= n / 2) start_j = m / 2 + 1;

            for (int j = start_j; j <= m; j++) {
                if (is_boundary(i, j)) continue;

                double A_v = -(v[i - 1][j] + v[i + 1][j]) * inv_h2
                    - (v[i][j - 1] + v[i][j + 1]) * inv_k2
                    + diag * v[i][j];

                double r = f[i][j] - A_v;

                residual[i][j] = r;
                max_residual_initial = std::max(max_residual_initial, fabs(r));
                sum_sq_residual_initial += r * r;
            }
        }

        initial_residual = max_residual_initial;
        double initial_residual_euclid = sqrt(sum_sq_residual_initial);

        std::cout << "Начальная невязка: max = " << std::scientific << initial_residual
            << ", евклидова = " << initial_residual_euclid << std::endl;

        free_arr(residual, n + 1);

        // МВР
        const double w1 = 1.0 - w;
        const double wd = w / diag;

        while (iter < Nmax) {
            eps_max = 0.0;

            for (int i = 1; i < n; i++) {
                int start_j = 1;
                if (i >= n / 2) start_j = m / 2 + 1;

                for (int j = start_j; j < m; j++) {
                    if (is_boundary(i, j)) continue;

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


        std::ofstream file_v("v_numerical.txt");
        file_v << std::scientific << std::setprecision(10);
        file_v << "# x y v_numerical\n";
        for (int i = 0; i <= n; i++) {
            double x = a + i * h;
            for (int j = 0; j <= m; j++) {
                if (is_in_domain(i, j)) {
                    double y = c + j * k;
                    file_v << x << " " << y << " " << v[i][j] << "\n";
                }
            }
        }
        file_v.close();


        std::ofstream file_u("u_exact.txt");
        file_u << std::scientific << std::setprecision(10);
        file_u << "# x y u_exact\n";
        for (int i = 0; i <= n; i++) {
            double x = a + i * h;
            for (int j = 0; j <= m; j++) {
                if (is_in_domain(i, j)) {
                    double y = c + j * k;
                    file_u << x << " " << y << " " << u_exact[i][j] << "\n";
                }
            }
        }
        file_u.close();

        // сохраняем погрешность
        std::ofstream file_err("error.txt");
        file_err << std::scientific << std::setprecision(10);
        file_err << "# x y error\n";
        for (int i = 0; i <= n; i++) {
            double x = a + i * h;
            for (int j = 0; j <= m; j++) {
                if (is_in_domain(i, j)) {
                    double y = c + j * k;
                    double error_val = fabs(u_exact[i][j] - v[i][j]);
                    file_err << x << " " << y << " " << error_val << "\n";
                }
            }
        }
        file_err.close();

        // ===== 3. ВЫЧИСЛЯЕМ ФИНАЛЬНУЮ НЕВЯЗКУ =====
        residual = alloc(n + 1, m + 1);
        double max_residual_final = 0.0;
        double sum_sq_residual_final = 0.0;
        int internal_nodes = 0;

        for (int i = 1; i < n; i++) {
            int start_j = 1;
            if (i >= n / 2) start_j = m / 2 + 1;

            for (int j = start_j; j < m; j++) {
                if (is_boundary(i, j)) continue;

                double A_v = (v[i - 1][j] + v[i + 1][j]) * inv_h2
                    + (v[i][j - 1] + v[i][j + 1]) * inv_k2
                    - diag * v[i][j];

                double r = f[i][j] - A_v;

                residual[i][j] = r;
                max_residual_final = std::max(max_residual_final, fabs(r));
                sum_sq_residual_final += r * r;
                internal_nodes++;
            }
        }

        residual_norm_max = max_residual_final;
        residual_norm_euclid = sqrt(sum_sq_residual_final);

        std::cout << "Финальная невязка: max = " << std::scientific << residual_norm_max
            << ", евклидова = " << residual_norm_euclid << std::endl;
        std::cout << "Количество внутренних узлов: " << internal_nodes << std::endl;

        free_arr(residual, n + 1);

        // погрешность
        double** error = alloc(n + 1, m + 1);
        error_node_i = 0;
        error_node_j = 0;
        error_norm_max = 0.0;

        for (int i = 0; i <= n; i++) {
            int start_j = 0;
            if (i > n / 2) start_j = m / 2;

            for (int j = start_j; j <= m; j++) {
                if (!is_in_domain(i, j)) continue;

                error[i][j] = fabs(u_exact[i][j] - v[i][j]);
                if (error[i][j] > error_norm_max) {
                    error_norm_max = error[i][j];
                    error_node_i = i;
                    error_node_j = j;
                }
            }
        }

        final_error = error_norm_max;
        free_arr(error, n + 1);

        auto end = std::chrono::high_resolution_clock::now();
        solve_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    }

    bool is_in_domain(int i, int j) const {
        if (i < 0 || i > n || j < 0 || j > m) return false;
        if (i > n / 2 && j < (m + 1) / 2) return false;  // вырезанный квадрант
        return true;
    }

    bool is_boundary(int i, int j) const {
        if (i == 0) return true; // сверху
        if (j == 0 && i <= n / 2) return true;     // слева      
        if (i == n && j >= (m + 1) / 2) return true;     // снизу         
        if (i == n / 2 && j <= (m + 1) / 2) return true;   // горизонтальный срез  
        if (j == (m + 1) / 2 && i >= (n + 1) / 2) return true;   // вертикальный срез
        if (j == m) return true;
        return false;
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

    void cleanup()
    {
        free_arr(v, n + 1);
        free_arr(f, n + 1);
        free_arr(u_exact, n + 1);
    }


    void printSolution() const noexcept {
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\nРешение:\n";
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                std::cout << std::setw(10) << u_exact[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

const double SORSolver::PI = acos(-1);

