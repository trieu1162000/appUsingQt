#ifndef NODE_H
#define NODE_H


class Node
{
public:
    Node(class QMainWindow* w);
    int x, y;
    int G; // distance from start
    int H; // distance to end
    int F; // g + h
    Node* parent_node = nullptr;
};

#endif // NODE_H
