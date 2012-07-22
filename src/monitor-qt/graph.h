#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QLinkedList>
#include <QDebug>

class Graph : public QWidget
{
    Q_OBJECT
public:
    explicit Graph(QWidget *parent = 0);

        int i_s_count;
        int i_stepsize;

signals:

public slots:

        void g_receivelist(QLinkedList<unsigned long> readlist, QLinkedList<unsigned long> writelist);




};

#endif // GRAPH_H
