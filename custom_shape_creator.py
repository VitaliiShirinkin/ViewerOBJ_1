import trimesh
import numpy as np

# Загрузите вашу модель OBJ
mesh = trimesh.load('model.obj')

# Получите все вершины и грани
vertices = mesh.vertices
faces = mesh.faces

# Запросите у пользователя количество угловых вершин
num_corners = int(input("Введите количество угловых вершин: "))

# Запросите у пользователя индексы угловых вершин
input_indices = input(f"Введите индексы угловых вершин (через запятую, например, 0,1,2,...,{num_corners-1}): ")
corner_indices = list(map(int, input_indices.split(',')))

# Проверяем, что введено корректное количество индексов
if len(corner_indices) != num_corners:
    print(f"Ошибка: необходимо ввести ровно {num_corners} индексов угловых вершин.")
    exit()

# Запросите у пользователя координаты угловых вершин
corner_coordinates = []
for i in range(num_corners):
    coords = input(f"Введите координаты вершины {corner_indices[i]} в формате (x, y, z): ")
    # Убираем скобки и разбиваем на компоненты
    coords = coords.strip('()')
    corner_coordinates.append(list(map(float, coords.split(','))))

# Проверка введенных координат
print("\nПроверка введенных данных:")
for i in range(num_corners):
    print(f"Вершина {corner_indices[i]}: координаты {corner_coordinates[i]}")

# Подтверждение ввода
confirm = input("Все данные введены верно? (да/нет): ")
if confirm.lower() != 'да':
    print("Пожалуйста, перезапустите скрипт и введите данные заново.")
    exit()

# Создаем новый массив вершин, включая только угловые
new_vertices = np.array(corner_coordinates)

# Создаем новые грани между угловыми вершинами
new_faces = []

if num_corners == 8:
    # Для параллелограмма (8 угловых вершин) создаем 12 треугольных граней
    for i in range(4):
        new_faces.append([i, i + 4, (i + 1) % 4 + 4])
        new_faces.append([i, (i + 1) % 4, (i + 1) % 4 + 4])
    for i in range(4):
        new_faces.append([i, (i + 1) % 4, i + 4])
else:
    # Для многоугольника создаем n-2 треугольника
    for i in range(1, num_corners - 1):
        new_faces.append([0, i, i + 1])

# Корректируем индексы для новых граней
new_faces = np.array(new_faces)

# Создаем новую модель
new_mesh = trimesh.Trimesh(vertices=new_vertices, faces=new_faces)

# Сохраняем новую модель в формате OBJ
new_mesh.export('new_model.obj')

print("Новая модель успешно сохранена как 'new_model.obj'.")
