# Lumina-EBook-Platform
Lumina 📖✨  Your Pocket-Sized, Smart Digital Library

The initial uml of the project:
@startuml eBookSystem

skinparam classAttributeIconSize 0
skinparam backgroundColor #FFFFFF
skinparam Handwritten false

package "Network Layer" {
    class Server {
        - serverSocket: QTcpServer*
        - clients: QList<QTcpSocket*>
        - dbManager: DatabaseManager&
        + startServer(port: int): bool
        + stopServer()
        - onNewConnection()
        - handleClientRequest(socket: QTcpSocket*, request: QString)
        - sendResponse(socket: QTcpSocket*, response: QString)
        - broadcastNotification(notification: QString, role: QString)
    }

    class ClientNetworkManager {
        - socket: QTcpSocket*
        + connectToServer(host: QString, port: int): bool
        + sendRequest(request: QString)
        - onReadyRead()
        - onDisconnected()
        + signal requestProcessed(response: QString)
    }
}

package "Core / Models" {
    enum UserRole {
        USER
        PUBLISHER
        ADMIN
    }

    abstract class User {
        # id: int
        # username: QString
        # passwordHash: QString
        # role: UserRole
        # isBlocked: bool
        + getId(): int
        + getUsername(): QString
        + getRole(): UserRole
        + checkBlockStatus(): bool
        + {abstract} displayDashboard()
    }

    class RegularUser {
        - favoriteGenres: QList<QString>
        - cart: Cart
        - library: PersonalLibrary
        + addFavoriteGenre(genre: QString)
        + getCart(): Cart&
        + getLibrary(): PersonalLibrary&
    }

    class Publisher {
        - companyName: QString
        - publishedBooks: QList<int>
        + getPublishedBooks(): QList<int>
    }

    class Admin {
        + blockUser(userId: int): bool
        + unblockUser(userId: int): bool
        + deleteBook(bookId: int): bool
    }

    class Book {
        - id: int
        - title: QString
        - author: QString
        - publisherId: int
        - genre: QString
        - description: QString
        - price: double
        - discountPercentage: double
        - pdfPath: QString
        - coverImagePath: QString
        - ratings: QList<int>
        + getFinalPrice(): double
        + getAverageRating(): double
        + updateDetails()
    }

    class Comment {
        - id: int
        - bookId: int
        - userId: int
        - username: QString
        - text: QString
        - timestamp: QDateTime
    }

    class Cart {
        - bookIds: QList<int>
        + addBook(bookId: int)
        + removeBook(bookId: int)
        + calculateTotal(booksMap: QMap<int, Book>): double
        + clear()
    }

    class PersonalLibrary {
        - ownedBookIds: QList<int>
        - savedBookIds: QList<int>
        - shelves: QMap<QString, QList<int>>
        + createShelf(name: QString)
        + addBookToShelf(shelfName: QString, bookId: int)
    }
}

package "Database / Storage" {
    class DatabaseManager {
        - db: QSqlDatabase
        + initDatabase(): bool
        + verifyUser(user: QString, pass: QString): User*
        + registerUser(user: QString, pass: QString, role: UserRole): bool
        + saveBook(book: Book): bool
        + getAllBooks(): QList<Book>
    }
}

' Relations
User <|-- RegularUser
User <|-- Publisher
User <|-- Admin

RegularUser "1" *-- "1" Cart
RegularUser "1" *-- "1" PersonalLibrary
Server "1" o-- "1" DatabaseManager
Server "1" o-- "*" QTcpSocket

@enduml
