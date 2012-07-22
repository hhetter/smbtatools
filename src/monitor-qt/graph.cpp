#include "graph.h"

Graph::Graph(QWidget *parent) :
    QWidget(parent)
{
        i_s_count = 0;
        i_stepsize=5;
}



void Graph::g_receivelist(QLinkedList<unsigned long> readlist,                           QLinkedList<unsigned long> writelist)
 {
                qDebug()<<"g_receivelist.readlist.size: "<< readlist.size();


 }




#include "graph.moc"
