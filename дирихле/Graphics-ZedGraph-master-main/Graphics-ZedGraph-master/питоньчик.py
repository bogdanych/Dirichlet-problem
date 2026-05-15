import numpy as np
import plotly.graph_objects as go

def load_data(filename):
    data = np.loadtxt(filename, skiprows=1)
    return data

def plot_surface_plotly(data, title, colorscale='Viridis'):
    x = data[:, 0]
    y = data[:, 1]
    z = data[:, 2]
    
    # Создаём сетку
    unique_x = np.unique(x)
    unique_y = np.unique(y)
    X, Y = np.meshgrid(unique_x, unique_y)
    Z = np.zeros_like(X)
    
    for i in range(len(x)):
        xi = np.where(unique_x == x[i])[0][0]
        yi = np.where(unique_y == y[i])[0][0]
        Z[yi, xi] = z[i]
    
    # Маскируем вырезанную область (x > 0, y < 0)
    for i in range(len(unique_x)):
        for j in range(len(unique_y)):
            if unique_x[i] > 0 and unique_y[j] < 0:
                Z[j, i] = None
    
    fig = go.Figure(data=[go.Surface(z=Z, x=unique_x, y=unique_y, 
                                     colorscale=colorscale)])
    
    fig.update_layout(
        title=title,
        scene=dict(
            xaxis_title='Y',
            yaxis_title='X',
            zaxis_title='Z',
            camera=dict(eye=dict(x=1.5, y=1.5, z=1.5))
        ),
        width=900,
        height=700
    )
    
    fig.show()

# Загружаем и строим
v_initial = load_data('Debug/v_initial.txt')
v_numerical = load_data('Debug/v_numerical.txt')
u_exact = load_data('Debug/u_exact.txt')
error = load_data('Debug/error.txt')

plot_surface_plotly(v_initial, 'Начальное приближение v(0)', 'RdBu')
plot_surface_plotly(v_numerical, 'Численное решение v(N)', 'Viridis')
plot_surface_plotly(u_exact, 'Точное решение u*', 'Plasma')
plot_surface_plotly(error, 'Разность точного и численного решения', 'Hot')