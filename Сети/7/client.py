import socket
import json

def send_request(socket, request):
    socket.sendall(json.dumps(request).encode())
    response = socket.recv(1024).decode()
    return json.loads(response)

def print_library(client_socket):
    request = {
        'action': 'get_library'
    }

    response = send_request(client_socket, request)
    if response['status'] == 'success':
        print("Библиотека:")
        for book in response['library']:
            print(book)
    else:
        print(response['message'])

def main():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(('127.0.0.1', 8888))

    while True:
        print("\n1. Добавить книгу и вывести библиотеку")
        print("2. Поиск книги")
        print("3. Удалить книгу")
        print("4. Сортировать библиотеку")
        print("5. Вывести всю библиотеку")
        print("0. Выход")

        choice = input("Выберите действие (0-5): ")

        if choice == '0':
            break

        elif choice == '1':
            title = input("Введите название книги: ")
            author = input("Введите автора книги: ")
            year = input("Введите год издания книги: ")

            request = {
                'action': 'add_book',
                'book': {'title': title, 'author': author, 'year': year}
            }

            response = send_request(client_socket, request)
            print(response['message'])

            # Вывести обновленную библиотеку
            print_library(client_socket)

        elif choice == '2':
            criteria = input("Введите критерий поиска: ")

            request = {
                'action': 'search_book',
                'criteria': criteria
            }

            response = send_request(client_socket, request)
            if response['status'] == 'success':
                print("Результат поиска:")
                for book in response['result']:
                    print(book)
            else:
                print(response['message'])

        elif choice == '3':
            title = input("Введите название книги для удаления: ")

            request = {
                'action': 'remove_book',
                'book': {'title': title}
            }

            response = send_request(client_socket, request)
            print(response['message'])

            # Вывести обновленную библиотеку
            print_library(client_socket)

        elif choice == '4':
            key = input("Введите ключ сортировки (например, title, author, year): ")

            request = {
                'action': 'sort_library',
                'key': key
            }

            response = send_request(client_socket, request)
            print(response['message'])

            # Вывести обновленную библиотеку
            print_library(client_socket)

        elif choice == '5':
            # Вывести всю библиотеку
            print_library(client_socket)

    client_socket.close()

if __name__ == "__main__":
    main()
