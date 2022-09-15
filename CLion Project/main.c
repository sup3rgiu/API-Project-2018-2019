#define _GNU_SOURCE // necessario per utilizzare fputs_unlocked()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { RED, BLACK };
typedef int COLOR;

typedef struct nodeEnt nodeEnt;
typedef struct nodeRel nodeRel;
typedef struct nodeDest nodeDest;

struct nodeEnt {
    char* key;
    COLOR color;
    struct nodeEnt* left;
    struct nodeEnt* right;
    struct nodeEnt* parent;
};

struct nodeDest {
    char* key;
    struct nodeEnt* data; // mittenti
    COLOR color;
    struct nodeDest* left;
    struct nodeDest* right;
    struct nodeDest* parent;
    int numRel;
};

struct nodeRel {
    char* key;
    struct nodeDest* data; // destinatari
    COLOR color;
    struct nodeRel* left;
    struct nodeRel* right;
    struct nodeRel* parent;
    char* maxRelations;
    int maxNumRel;
};


nodeEnt *nilNodeEnt = NULL;
nodeDest *nilNodeDest = NULL;
nodeRel *nilNodeRel = NULL;
nodeEnt *rootEnt = NULL; // radice albero entità
nodeRel *rootRel = NULL; // radice albero relazioni

/* FUNZIONI RB ENTITA' */

void left_rotate_ent(nodeEnt **root, nodeEnt *x) {
    nodeEnt *y = x->right;
    x->right = y->left;

    if(y->left != nilNodeEnt)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == nilNodeEnt)
        (*root) = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void right_rotate_ent(nodeEnt **root, nodeEnt *x) {
    nodeEnt *y = x->left;
    x->left = y->right;

    if(y->right != nilNodeEnt)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == nilNodeEnt)
        (*root) = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;
}

void insertFixup_ent(nodeEnt **root, nodeEnt *z) {
    while(z->parent->color == RED) {
        nodeEnt *y = NULL;
        if(z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate_ent(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate_ent(root, z->parent->parent);
            }
        }

        else {
            y = z->parent->parent->left;
            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate_ent(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate_ent(root, z->parent->parent);
            }
        }

    }
    (*root)->color = BLACK;
}

nodeEnt * insert_ent(nodeEnt **root, char *key) {
    nodeEnt *y = nilNodeEnt;
    nodeEnt *x = (*root);
    nodeEnt *z = malloc(sizeof(nodeEnt));
    z->key = strdup(key);
    while(x != nilNodeEnt) {
        y = x;

        if (strcmp(z->key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if(y == nilNodeEnt)
        (*root) = z;
    else if(strcmp(z->key, y->key) < 0)
        y->left = z;
    else
        y->right = z;

    z->left = nilNodeEnt;
    z->right = nilNodeEnt;
    z->color = RED;
    insertFixup_ent(root, z);
    return z;
}

void deleteFixup_ent(nodeEnt **root, nodeEnt *x) {
    while(x != (*root) && x->color == BLACK) {
        nodeEnt *w = NULL;
        if(x == x->parent->left) {
            w = x->parent->right;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate_ent(root, x->parent);
                w = x->parent->right;
            }

            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate_ent(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate_ent(root, x->parent);
                x = (*root);

            }
        }

        else {
            w = x->parent->left;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate_ent(root, x->parent);
                w = x->parent->left;
            }

            if(w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if(w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate_ent(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate_ent(root, x->parent);
                x = (*root);
            }
        }

    }
    x->color = BLACK;
}

nodeEnt * tree_minimum_ent(nodeEnt *x) {
    while(x->left != nilNodeEnt)
        x = x->left;
    return x;
}

nodeEnt * tree_successor_ent(nodeEnt *x) {
    if(x->right != nilNodeEnt)
        return tree_minimum_ent(x->right);

    nodeEnt *y = x->parent;
    while(y != nilNodeEnt && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

nodeEnt * delete_ent(nodeEnt **root, nodeEnt *z) {
    nodeEnt *y = NULL;
    nodeEnt *x = NULL;
    if(z->left == nilNodeEnt || z->right == nilNodeEnt)
        y = z;
    else
        y = tree_successor_ent(z);

    if(y->left != nilNodeEnt)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;

    if(y->parent == nilNodeEnt)
        (*root) = x;
    else {
        if(y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }

    if(y != z) {
        z->key = strdup(y->key);
    }

    if(y->color == BLACK)
        deleteFixup_ent(root, x);

    free(y->key);
    free(y);
    return nilNodeEnt;
    //return y;
}

nodeEnt * search_ent(nodeEnt *x, char *key) {
    while(x != nilNodeEnt && strcmp(key, x->key) != 0) {
        if(strcmp(key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}

void inorder_ent(nodeEnt *root)
{
    if (root == nilNodeEnt)
        return;
    inorder_ent(root->left);
    printf("%s\n", root->key);
    inorder_ent(root->right);
}

/* FUNZIONI RB DESTINATARI */

void left_rotate_dest(nodeDest **root, nodeDest *x) {
    nodeDest *y = x->right;
    x->right = y->left;

    if(y->left != nilNodeDest)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == nilNodeDest)
        (*root) = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void right_rotate_dest(nodeDest **root, nodeDest *x) {
    nodeDest *y = x->left;
    x->left = y->right;

    if(y->right != nilNodeDest)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == nilNodeDest)
        (*root) = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;
}

void insertFixup_dest(nodeDest **root, nodeDest *z) {
    while(z->parent->color == RED) {
        nodeDest *y = NULL;
        if(z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate_dest(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate_dest(root, z->parent->parent);
            }
        }

        else {
            y = z->parent->parent->left;
            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate_dest(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate_dest(root, z->parent->parent);
            }
        }

    }
    (*root)->color = BLACK;
}

nodeDest * insert_dest(nodeDest **root, char *key) {
    nodeDest *y = nilNodeDest;
    nodeDest *x = (*root);
    nodeDest *z = malloc(sizeof(nodeDest));
    z->key = strdup(key);
    z->data = nilNodeEnt;
    z->numRel = 0;
    while(x != nilNodeDest) {
        y = x;

        if (strcmp(z->key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if(y == nilNodeDest)
        (*root) = z;
    else if(strcmp(z->key, y->key) < 0)
        y->left = z;
    else
        y->right = z;

    z->left = nilNodeDest;
    z->right = nilNodeDest;
    z->color = RED;
    insertFixup_dest(root, z);
    return z;
}

void deleteFixup_dest(nodeDest **root, nodeDest *x) {
    while(x != (*root) && x->color == BLACK) {
        nodeDest *w = NULL;
        if(x == x->parent->left) {
            w = x->parent->right;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate_dest(root, x->parent);
                w = x->parent->right;
            }

            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate_dest(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate_dest(root, x->parent);
                x = (*root);

            }
        }

        else {
            w = x->parent->left;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate_dest(root, x->parent);
                w = x->parent->left;
            }

            if(w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if(w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate_dest(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate_dest(root, x->parent);
                x = (*root);
            }
        }

    }
    x->color = BLACK;
}

nodeDest * tree_minimum_dest(nodeDest *x) {
    while(x->left != nilNodeDest)
        x = x->left;
    return x;
}

nodeDest * tree_successor_dest(nodeDest *x) {
    if(x->right != nilNodeDest)
        return tree_minimum_dest(x->right);

    nodeDest *y = x->parent;
    while(y != nilNodeDest && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

nodeDest * delete_dest(nodeDest **root, nodeDest *z) {
    nodeDest *y = NULL;
    nodeDest *x = NULL;
    if(z->left == nilNodeDest || z->right == nilNodeDest)
        y = z;
    else
        y = tree_successor_dest(z);

    if(y->left != nilNodeDest)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;

    if(y->parent == nilNodeDest)
        (*root) = x;
    else {
        if(y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }

    if(y != z) {
        z->key = strdup(y->key);
        z->data = y->data;
        z->numRel = y->numRel;
    }

    if(y->color == BLACK)
        deleteFixup_dest(root, x);

    free(y->key);
    free(y);
    return nilNodeDest;
    //return y;
}

nodeDest * search_dest(nodeDest *x, char *key) {
    while(x != nilNodeDest && strcmp(key, x->key) != 0) {
        if(strcmp(key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}

void inorder_dest(nodeDest *root)
{
    if (root == nilNodeDest)
        return;
    inorder_dest(root->left);
    printf("%s\n", root->key);
    inorder_dest(root->right);
}

/* FUNZIONI RB RELAZIONI */

void left_rotate_rel(nodeRel **root, nodeRel *x) {
    nodeRel *y = x->right;
    x->right = y->left;

    if(y->left != nilNodeRel)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == nilNodeRel)
        (*root) = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void right_rotate_rel(nodeRel **root, nodeRel *x) {
    nodeRel *y = x->left;
    x->left = y->right;

    if(y->right != nilNodeRel)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == nilNodeRel)
        (*root) = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;
}

void insertFixup_rel(nodeRel **root, nodeRel *z) {
    while(z->parent->color == RED) {
        nodeRel *y = NULL;
        if(z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate_rel(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate_rel(root, z->parent->parent);
            }
        }

        else {
            y = z->parent->parent->left;
            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate_rel(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate_rel(root, z->parent->parent);
            }
        }

    }
    (*root)->color = BLACK;
}

nodeRel * insert_rel(nodeRel **root, char *key) {
    nodeRel *y = nilNodeRel;
    nodeRel *x = (*root);
    nodeRel *z = malloc(sizeof(nodeRel));
    z->key = strdup(key);
    z->data = nilNodeDest;
    z->maxNumRel = 0;
    z->maxRelations = NULL;
    while(x != nilNodeRel) {
        y = x;

        if (strcmp(z->key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if(y == nilNodeRel)
        (*root) = z;
    else if(strcmp(z->key, y->key) < 0)
        y->left = z;
    else
        y->right = z;

    z->left = nilNodeRel;
    z->right = nilNodeRel;
    z->color = RED;
    insertFixup_rel(root, z);
    return z;
}

void deleteFixup_rel(nodeRel **root, nodeRel *x) {
    while(x != (*root) && x->color == BLACK) {
        nodeRel *w = NULL;
        if(x == x->parent->left) {
            w = x->parent->right;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate_rel(root, x->parent);
                w = x->parent->right;
            }

            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate_rel(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate_rel(root, x->parent);
                x = (*root);

            }
        }

        else {
            w = x->parent->left;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate_rel(root, x->parent);
                w = x->parent->left;
            }

            if(w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if(w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate_rel(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate_rel(root, x->parent);
                x = (*root);
            }
        }

    }
    x->color = BLACK;
}

nodeRel * tree_minimum_rel(nodeRel *x) {
    while(x->left != nilNodeRel)
        x = x->left;
    return x;
}

nodeRel * tree_successor_rel(nodeRel *x) {
    if(x->right != nilNodeRel)
        return tree_minimum_rel(x->right);

    nodeRel *y = x->parent;
    while(y != nilNodeRel && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

nodeRel * delete_rel(nodeRel **root, nodeRel *z) {
    nodeRel *y = NULL;
    nodeRel *x = NULL;
    if(z->left == nilNodeRel || z->right == nilNodeRel)
        y = z;
    else
        y = tree_successor_rel(z);

    if(y->left != nilNodeRel)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;

    if(y->parent == nilNodeRel)
        (*root) = x;
    else {
        if(y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    }

    if(y != z) {
        z->key = strdup(y->key);
        z->data = y->data;
        z->maxNumRel = y->maxNumRel;
        if(y->maxRelations != NULL) // strdup non accetta NULL come parametro
            z->maxRelations = strdup(y->maxRelations);
        else
            z->maxRelations = y->maxRelations;
    }

    if(y->color == BLACK)
        deleteFixup_rel(root, x);

    free(y->key);
    free(y->maxRelations);
    y->maxRelations = NULL;
    free(y);
    return nilNodeRel;
    //return y;
}

nodeRel * search_rel(nodeRel *x, char *key) {
    while(x != nilNodeRel && strcmp(key, x->key) != 0) {
        if(strcmp(key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}

void inorder_rel(nodeRel *root)
{
    if (root == nilNodeRel)
        return;
    inorder_rel(root->left);
    printf("%s\n", root->key);
    inorder_rel(root->right);
}

/* FUNZIONI GESTIONE COMANDI */

void updateMaxIfNeeded(nodeRel *rel, nodeDest *dest)
{
    if(dest->numRel < rel->maxNumRel) // aggiornamento massimo non richiesto
        return;
    else if(dest->numRel > rel->maxNumRel) { // se è maggiore, allora attualmente dest sarà l'unico massimo
        rel->maxNumRel = dest->numRel;
        free(rel->maxRelations);
        rel->maxRelations = strdup(dest->key); // EVENTUALMENTE FARNE UNA COPIA con strdup
    }
    else { // vuol dire che c'è più di un'entità come massimo. Setto a NULL e faccio l'ispezione direttamente nel report --> MIGLIORABILE
        free(rel->maxRelations);
        rel->maxRelations = NULL;
    }
}

void addrel(char *id_orig, char *id_dest, char *id_rel) {

    nodeEnt *orig = search_ent(rootEnt, id_orig);
    if(orig == nilNodeEnt) // entità di origine non esistente
        return;

    nodeEnt *dest = search_ent(rootEnt, id_dest);
    if(dest == nilNodeEnt) // entità di destinazione non esistente
        return;

    nodeRel *rel = search_rel(rootRel, id_rel);
    if(rel == nilNodeRel) { // relazione non ancora esistente
        rel = insert_rel(&rootRel, id_rel); // inserisco relazione
        ++rel->maxNumRel; // al primo inserimento aggiorno (metto a 1) il massimo
        rel->maxRelations = strdup(id_dest); // al primo inserimento metto il id_dest come entità con max entranti

        nodeDest *destInRel = insert_dest(&rel->data, id_dest); // inserisco destinatario
        ++destInRel->numRel; // aggiorno il numero di mittenti per quel destinatario e per quella relazione
        insert_ent(&destInRel->data, id_orig); // inserisco mittente nel destinatario per quella relazione
        return;
    }
    else { // identificativo relazione già esistente
        nodeDest *destInRel = search_dest(rel->data, id_dest); // cerco se esiste già il destinatario in quel tipo di relazione
        if (destInRel != nilNodeDest) { // destinatario già esistente
            nodeEnt *origInRel = search_ent(destInRel->data, id_orig); // cerco se esiste già il mittente
            if (origInRel != nilNodeEnt) // mittente già esistente
                return; // non faccio nulla perchè la relazione esiste già
            else {
                insert_ent(&destInRel->data, id_orig); // altrimenti inserisco il mittente
                ++destInRel->numRel; // aggiorno il numero di releazioni per il destinatario
                updateMaxIfNeeded(rel, destInRel);
            }
        }
        else { // destinatario non ancora esistente
            destInRel = insert_dest(&rel->data, id_dest); // inserisco destinatario
            ++destInRel->numRel;
            insert_ent(&destInRel->data, id_orig); // inserisco mittente
            updateMaxIfNeeded(rel, destInRel);
        }

    }

}

void addent(char *id_ent) {
    nodeEnt *x = search_ent(rootEnt, id_ent);
    if(x == nilNodeEnt) { // entità non ancora monitorata
        insert_ent(&rootEnt, id_ent);
    }
}

int newMax = 0;
char* maxRelation = NULL;

// fa una visita in ordine dei destinatari di una relazione
void findNewMax(nodeDest* root) {
    if (root == nilNodeDest)
        return;

    findNewMax(root->left);

    /* PER OGNI DESTINATARIO */
    if(root->numRel > newMax) { // l'entità è il nuovo massimo assoulto
        newMax = root->numRel;
        maxRelation = root->key;
    }
    else if(root->numRel == newMax) // l'entità è massimo a parimerito con altre
        maxRelation = NULL; // setto a NULL e faccio l'ispezione direttamente nel report --> MIGLIORABILE
    /* PER OGNI DESTINATARIO */

    findNewMax(root->right);
}

void buildMaxForRel(nodeRel *rel) {
    newMax = 0;
    maxRelation = NULL;
    findNewMax(rel->data);
    rel->maxNumRel = newMax;
    if(maxRelation == NULL) { // più entità sono massimo
        free(rel->maxRelations);
        rel->maxRelations = NULL; // setto a NULL e faccio l'ispezione direttamente nel report --> MIGLIORABILE
    }
    else { // una sola entità è massimo
        free(rel->maxRelations);
        rel->maxRelations = strdup(maxRelation);
    }
}

void delrel(char *id_orig, char *id_dest, char *id_rel) {
    nodeRel *rel = search_rel(rootRel, id_rel);
    if(rel == nilNodeRel) // relazione non esistente
        return;

    nodeDest *destInRel = search_dest(rel->data, id_dest);
    if(destInRel == nilNodeDest) // destinatario non esistente
        return;

    nodeEnt *origInRel = search_ent(destInRel->data, id_orig);
    if(origInRel == nilNodeEnt) // mittente non esistente
        return;
    else {
        delete_ent(&destInRel->data, origInRel); // cancello mittente nel destinatario
        --destInRel->numRel; // decremento il numero di relazioni per il destinatario

        if((destInRel->numRel + 1) == rel->maxNumRel) { // se era un massimo -> aggiornamento massimo richiesto
            buildMaxForRel(rel);
        }

        if(destInRel->numRel == 0) { // l'entità di destinazione non ha più mittenti
            delete_dest(&rel->data, destInRel);
        }

        //if(rel->maxNumRel == 0) { // la relazione non ha più destinatari
        if(rel->data == nilNodeDest) { // la relazione non ha più destinatari
            delete_rel(&rootRel, rel);
        }
    }
}

// visita in post-ordine dei mittenti di un destinatario
void deleteAllMittenti(nodeEnt *root) {
    if (root == nilNodeEnt)
        return;

    deleteAllMittenti(root->left);
    deleteAllMittenti(root->right);

    free(root->key);
    free(root);
}

int updateMaxDelentRequired = 0;
char **destNodesToDelete = NULL;
int sizeArrayDest = 0;

void updateMittentiDelent(nodeDest *root, nodeRel *rel, char *id_ent) {
    if(strcmp(root->key, id_ent) == 0) { // entità da eliminare presente come destinatario
        deleteAllMittenti(root->data); // cancello tutti i mittenti
        if(root->numRel == rel->maxNumRel) // se era un massimo
            updateMaxDelentRequired = 1;

        // aggiungo nei destinatari da cancellare
        sizeArrayDest++;
        destNodesToDelete = realloc(destNodesToDelete, sizeArrayDest * sizeof(char*));
        destNodesToDelete[sizeArrayDest-1] = strdup(root->key);
    }
    else {
        nodeEnt *entToDelete = search_ent(root->data, id_ent); // cerco l'entità da eliminare nei mittenti
        if(entToDelete != nilNodeEnt) {
            delete_ent(&root->data, entToDelete); // elimino il mittente
            if(root->numRel == rel->maxNumRel) // se era un massimo
                updateMaxDelentRequired = 1;
            --root->numRel; // decremento il numero di relazioni per il destinatario
            if(root->numRel == 0) { // se il destinatario non ha più mittenti
                // aggiungo nei destinatari da cancellare
                sizeArrayDest++;
                destNodesToDelete = realloc(destNodesToDelete, sizeArrayDest * sizeof(char*));
                destNodesToDelete[sizeArrayDest-1] = strdup(root->key);
            }
        }
    }

}

// fa una visita in ordine dei destinatari per la relazione in esame
void updateRelationDelent(nodeDest *root, nodeRel *rel, char *id_ent) {
    if (root == nilNodeDest)
        return;

    updateRelationDelent(root->left, rel, id_ent);
    updateMittentiDelent(root, rel, id_ent); // per ogni destinatario
    updateRelationDelent(root->right, rel, id_ent);
}

char **relNodesToDelete = NULL;
int sizeArrayRel = 0;

void updateRelationsDelent(nodeRel *root, char *id_ent) {
    if (root == nilNodeRel)
        return;

    updateRelationsDelent(root->left, id_ent);

    /* PER OGNI RELAZIONE */
    updateMaxDelentRequired = 0;
    destNodesToDelete = NULL; // array di puntatori a stringhe che conterrà i nomi dei destinatari senza mittenti e quindi da cancellare
    sizeArrayDest = 0;

    updateRelationDelent(root->data, root, id_ent);

    // cancello destinatari senza mittenti
    if(sizeArrayDest > 0) {
        for (int i = 0; i < sizeArrayDest; ++i) {
            nodeDest *toDelete = search_dest(root->data, destNodesToDelete[i]);
            delete_dest(&root->data, toDelete);
            free(destNodesToDelete[i]);
        }
    }

    if(root->data == nilNodeDest) { // se la relazione non ha più destinatari
        // aggiungo nelle relazioni da cancellare
        sizeArrayRel++;
        relNodesToDelete = realloc(relNodesToDelete, sizeArrayRel * sizeof(char*));
        relNodesToDelete[sizeArrayRel-1] = strdup(root->key);
        updateMaxDelentRequired = 0; // non dovrò aggiornare il massimo per tale relazione visto che la cancellerò
    }

    if(updateMaxDelentRequired == 1) {
        buildMaxForRel(root);
    }

    free(destNodesToDelete);
    /* PER OGNI RELAZIONE */

    updateRelationsDelent(root->right, id_ent);
}

void delent(char *id_ent) {
    nodeEnt *ent = search_ent(rootEnt, id_ent);
    if(ent == nilNodeEnt) { // entità non esistente
        return;
    }
    else {
        delete_ent(&rootEnt, ent); // rimuovo l'entità dall'albero delle entità
        relNodesToDelete = NULL; // array di puntatori a stringhe che conterrà i nomi delle relazioni senza destinatari e quindi da cancellare
        sizeArrayRel = 0;

        updateRelationsDelent(rootRel, id_ent);

        // cancello relazioni senza destinatari
        if(sizeArrayRel > 0) {
            for (int i = 0; i < sizeArrayRel; ++i) {
                nodeRel *toDelete = search_rel(rootRel, relNodesToDelete[i]);
                delete_rel(&rootRel, toDelete);
                free(relNodesToDelete[i]);
            }
        }

        free(relNodesToDelete);
    }
}

// fa una visita in ordine dei destinatari della relazione in considerazione
void printMaxForRel(nodeDest *root, int maxNum) {
    if (root == nilNodeDest)
        return;

    printMaxForRel(root->left, maxNum);
    if(root->numRel == maxNum) { // se l'entità ha un numero di mittenti pari a quello del massimo
        fputs_unlocked("\"", stdout);
        fputs_unlocked(root->key, stdout);
        fputs_unlocked("\" ", stdout);
    }
    printMaxForRel(root->right, maxNum);
}

int first = 1;

// funzione per stampare gli int con fputc
void printInt(int n)
{
    if( n > 9 )
    { int a = n / 10;

        n -= 10 * a;
        printInt(a);
    }
    fputc_unlocked('0'+n, stdout);
}

void reportRel(nodeRel *rel) {
    if(first != 1)
        fputs_unlocked(" ", stdout);

    fputs_unlocked("\"", stdout);
    fputs_unlocked(rel->key, stdout);
    fputs_unlocked("\" ", stdout);
    if(rel->maxRelations != NULL) { // una sola entità come massimo per la relazione in considerazione
        fputs_unlocked("\"", stdout);
        fputs_unlocked(rel->maxRelations, stdout);
        fputs_unlocked("\" ", stdout);
    }
    else { // più di una entità come massimo per la relazione in considerazione
        printMaxForRel(rel->data, rel->maxNumRel);
    }
    //printf("%d;", rel->maxNumRel);
    printInt(rel->maxNumRel);
    fputc_unlocked(';', stdout);
}

// fa una visita in ordine
void report(nodeRel *root) {
    if (root == nilNodeRel)
        return;

    report(root->left);

    reportRel(root); // per ogni relazione
    first = 0;

    report(root->right);
}

#define MAXBUFFER 1000

int main() {

    /* INIT NIL NODES */
    nilNodeEnt = malloc(sizeof(nodeEnt));
    nilNodeEnt->color = BLACK;
    nilNodeEnt->key = NULL;
    nilNodeEnt->left = nilNodeEnt->right = nilNodeEnt->parent = nilNodeEnt;

    nilNodeDest = malloc(sizeof(nodeDest));
    nilNodeDest->color = BLACK;
    nilNodeDest->key = NULL;
    nilNodeDest->data = NULL;
    nilNodeDest->numRel = 0;
    nilNodeDest->left = nilNodeDest->right = nilNodeDest->parent = nilNodeDest;

    nilNodeRel = malloc(sizeof(nodeRel));
    nilNodeRel->color = BLACK;
    nilNodeRel->key = NULL;
    nilNodeRel->data = NULL;
    nilNodeRel->maxNumRel = 0;
    nilNodeRel->maxRelations = NULL;
    nilNodeRel->left = nilNodeRel->right = nilNodeRel->parent = nilNodeRel;

    /* INIT ROOT NODES */
    rootEnt = nilNodeEnt;
    rootRel = nilNodeRel;


    /* RIMUOVERE IN PRODUZIONE */
    // serve per far funzionare stdin con Clion
    freopen("input.txt","r",stdin);
    /* RIMUOVERE IN PRODUZIONE*/

    char s[MAXBUFFER]; // stringa che conterrà una sola riga dell'input
    const char delimiter[2] = " "; // delimitatore per dividere la stringa in parole


    while (fgets(s, MAXBUFFER, stdin) != NULL)
    {
        char *words[4] = {NULL, NULL, NULL, NULL}; // array di 4 stringhe

        s[strlen(s) - 1] = 0; // rimuove \n dalla stringa (non fa controlli sulla sua presenza, ma in input c'è sempre \n al termine della linea)

        int i = -1;
        words[++i] = strtok(s, delimiter); // prima parola della stringa (corrisponde al comando)

        // individua le altre parole
        while((words[++i] = strtok(NULL, delimiter)) != NULL) {
            words[i]++; // rimuove le " all'inizio della stringa
            words[i][strlen(words[i]) - 1] = 0; // rimuove le " alla fine della stringa
        }

        if(strcmp(words[0], "addent") == 0) {
            addent(words[1]);
        } else if (strcmp(words[0], "addrel") == 0) {
            addrel(words[1], words[2], words[3]);
        } else if (strcmp(words[0], "delent") == 0) {
            delent(words[1]);
        } else if (strcmp(words[0], "delrel") == 0) {
            delrel(words[1], words[2], words[3]);
        } else if (strcmp(words[0], "report") == 0) {
            if(rootRel == nilNodeRel)
                fputs_unlocked("none\n", stdout);
            else {
                first = 1; // utilizzato per mettere lo spazio dopo il punto e virgola se ci sono più rel da stampare nel report
                report(rootRel);
                fputc_unlocked('\n', stdout);

            }
        } else { // "end"
            return 0;
        }

    }

    return 0;

}
