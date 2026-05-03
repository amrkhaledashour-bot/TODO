#ifndef ASYNC_EXECUTOR_H
#define ASYNC_EXECUTOR_H

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <functional>

/**
 * @brief Runs database operations off the UI thread.
 *
 * Wraps QtConcurrent::run with QFutureWatcher so we can fire
 * a callback on the main thread when the work is done.
 * This keeps the UI responsive during heavy DB operations.
 */
class AsyncExecutor : public QObject {
    Q_OBJECT

public:
    explicit AsyncExecutor(QObject* parent = nullptr) : QObject(parent) {}

    /**
     * @brief Run a function asynchronously and call back on the main thread.
     *
     * Usage:
     *   executor->run<QVector<Task>>(
     *       [&]() { return db.getTasks(projectId); },
     *       [this](const QVector<Task>& tasks) { handleTasks(tasks); }
     *   );
     *
     * @tparam T         Return type of the operation
     * @param operation  Lambda that does the actual work (runs on worker thread)
     * @param callback   Lambda called with the result (runs on main thread)
     */
    template<typename T>
    void run(std::function<T()> operation, std::function<void(const T&)> callback) {
        auto* watcher = run<T>(std::move(operation));
        connect(watcher, &QFutureWatcher<T>::finished, this, [watcher, callback]() {
            callback(watcher->result());
            watcher->deleteLater();
        });
    }

    /**
     * @brief Run a function asynchronously and return the watcher.
     * 
     * Caller is responsible for connecting to 'finished' and potentially 
     * calling deleteLater() on the watcher.
     * 
     * @tparam T         Return type of the operation
     * @param operation  Lambda that does the actual work
     * @return QFutureWatcher<T>* The watcher object.
     */
    template<typename T>
    QFutureWatcher<T>* run(std::function<T()> operation) {
        QFuture<T> future = QtConcurrent::run(std::move(operation));
        auto* watcher = new QFutureWatcher<T>(this);
        watcher->setFuture(future);
        return watcher;
    }

    /**
     * @brief Run a void operation asynchronously with a completion callback.
     * @param operation  Lambda that does work (worker thread)
     * @param callback   Called when done (main thread)
     */
    void runVoid(std::function<void()> operation, std::function<void()> callback = nullptr) {
        QFuture<void> future = QtConcurrent::run(std::move(operation));

        auto* watcher = new QFutureWatcher<void>(this);

        connect(watcher, &QFutureWatcher<void>::finished, this, [watcher, callback]() {
            if (callback) callback();
            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }
};

#endif // ASYNC_EXECUTOR_H
