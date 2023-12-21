import socket
import multiprocessing
import json
import sqlite3

DB_FILE = 'library.db'

def initialize_database():
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            year INTEGER NOT NULL
        )
    ''')

    conn.commit()
    conn.close()

def handle_client(client_socket):
    library = LibraryDatabase(DB_FILE)

    while True:
        data = client_socket.recv(1024).decode()
        if not data:
            break

        request = json.loads(data)
        response = process_request(request, library)

        client_socket.send(json.dumps(response).encode())

    client_socket.close()

class LibraryDatabase:
    def __init__(self, db_file):
        self.db_file = db_file

    def add_book(self, book):
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()

        cursor.execute('INSERT INTO books (title, author, year) VALUES (?, ?, ?)',
                       (book['title'], book['author'], book['year']))

        conn.commit()
        conn.close()

    def get_library(self):
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()

        cursor.execute('SELECT * FROM books')
        result = cursor.fetchall()

        conn.close()

        return result

    def search_book(self, criteria):
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()

        cursor.execute('SELECT * FROM books WHERE title LIKE ? OR author LIKE ? OR year LIKE ?',
                       ('%' + criteria + '%', '%' + criteria + '%', '%' + criteria + '%'))
        result = cursor.fetchall()

        conn.close()

        return result

    def remove_book(self, title):
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()

        cursor.execute('DELETE FROM books WHERE title = ?', (title,))
        conn.commit()

        conn.close()

def process_request(request, library):
    action = request.get('action')

    if action == 'add_book':
        book = request.get('book')
        library.add_book(book)
        return {'status': 'success', 'message': 'Book added successfully.'}

    elif action == 'search_book':
        criteria = request.get('criteria')
        result = library.search_book(criteria)
        return {'status': 'success', 'result': result}

    elif action == 'remove_book':
        title = request.get('book', {}).get('title', '')
        library.remove_book(title)
        return {'status': 'success', 'message': 'Book removed successfully.'}

    elif action == 'get_library':
        library_data = library.get_library()
        return {'status': 'success', 'library': library_data}

    # Другие методы для работы с базой данных

    else:
        return {'status': 'error', 'message': 'Invalid action.'}

def main():
    initialize_database()

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 5000))
    server_socket.listen(5)

    print("Server is listening on port 5000...")

    while True:
        client_socket, addr = server_socket.accept()
        print(f"Accepted connection from {addr}")

        client_handler = multiprocessing.Process(target=handle_client, args=(client_socket,))
        client_handler.start()

if __name__ == "__main__":
    main()
