#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED

#include "list.h"

struct weight_head;

typedef struct neuron {
	/* A list of weights, sorted by the corresponding input. */
    struct weight_head *weights;
    /* The current output of the neuron. */
    double output;
    /* The sum of the weighted inputs. */
    double net_sum;
    /* The used transfer function. */
    double (*transfer) (double);
} *pNeuron;

typedef struct weight {
	double value;
	double delta;
	pNeuron from;
} *pWeight;

typedef struct weight_head {
	pWeight weight;
	struct weight_head *next;
} *weight_head;

typedef struct neuron_head {
	unsigned int index;

	pNeuron current_neuron;
	struct neuron_head *prev;
	struct neuron_head *next;
} *neuron_head;

pNeuron create_random_neuron (neuron_head input_neuron_head);
neuron_head create_neuron_list (unsigned int neuron_count, neuron_head prev_layer_neurons);
void free_neuron (pNeuron cur_neuron);

#endif // NEURON_H_INCLUDED
