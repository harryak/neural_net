#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "neuron.h"

/**
 *	Generate a random floating point number from min to max.
 */
double randfrom (double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

/**
 *  Creates a weight node for a weight-list. Not linked!
 */
weight_head create_weight_node (double value, pNeuron from) {
    pWeight cur_weight = (pWeight) malloc (sizeof(struct weight));
    if (NULL == cur_weight) {
        perror ("Not enough memory to create weight.");
        exit (EXIT_FAILURE);
    }

    cur_weight->value = value;
    cur_weight->delta = 0;
    cur_weight->from  = from;

    weight_head cur_weight_head = (weight_head) malloc (sizeof(struct weight_head));
    cur_weight_head->weight = cur_weight;
    cur_weight_head->next   = NULL;

    return cur_weight_head;
}

void free_weight_list (weight_head weights) {
	if (NULL == weights) return;

	weight_head tmp;

	do {
		free (weights->weight);

		tmp = weights->next;
		free (weights);
		weights = tmp;
	} while (NULL != weights);
}

double id (double a) {
	return a;
}

neuron_head create_input_neurons (list_head input_vector) {
	if (NULL == input_vector) {
		perror ("Empty input vector. Can't create new neuron.");
		exit (EXIT_FAILURE);
	}

	list_head tmp = input_vector;

	pNeuron cur_neuron;
	neuron_head cur_neuron_head, first_neuron_head, prev_neuron_head;
	unsigned int i = 1;

	cur_neuron = (pNeuron) malloc (sizeof(struct neuron));
	if (NULL == cur_neuron) {
		perror ("Not enough memory to create neuron.");
		exit (EXIT_FAILURE);
	}

	cur_neuron->output = input_vector->value;
	cur_neuron->net_sum = 0;
	cur_neuron->transfer = &id;
	cur_neuron->weights = NULL;
	input_vector = input_vector->next;

	first_neuron_head = (neuron_head) malloc (sizeof(struct neuron_head));
	first_neuron_head->current_neuron = cur_neuron;
	first_neuron_head->index = 0;
	prev_neuron_head = first_neuron_head;

	while (NULL != input_vector) {
		cur_neuron = (pNeuron) malloc (sizeof(struct neuron));
		if (NULL == cur_neuron) {
			perror ("Not enough memory to create neuron.");
			exit (EXIT_FAILURE);
		}

		// Init standard values.
		cur_neuron->output = input_vector->value;
		cur_neuron->net_sum = 0;
		cur_neuron->transfer = &id;
		cur_neuron->weights = NULL;

		cur_neuron_head = (neuron_head) malloc (sizeof(struct neuron_head));
		cur_neuron_head->current_neuron = cur_neuron;
		cur_neuron_head->index = i;
		cur_neuron_head->prev  = prev_neuron_head;
		prev_neuron_head->next = cur_neuron_head;

		prev_neuron_head = cur_neuron_head;
		i++;
		input_vector = input_vector->next;
	}

	cur_neuron_head->next   = first_neuron_head;
	first_neuron_head->prev = cur_neuron_head;

	input_vector = tmp;

	return first_neuron_head;
}

/**
 *	Initialize a neuron with random weights and tanh as transfer function.
 *	Has to be given the number of inputs.
 */
pNeuron create_random_neuron (neuron_head input_neuron_head) {
	if (NULL == input_neuron_head) {
		perror ("Empty list of input neurons. Can't create new neuron.");
		exit (EXIT_FAILURE);
	}

    pNeuron cur_neuron = (pNeuron) malloc (sizeof(struct neuron));
    if (NULL == cur_neuron) {
        perror ("Not enough memory to create neuron.");
        exit (EXIT_FAILURE);
    }

	// Init standard values.
    cur_neuron->output = 0;
    cur_neuron->net_sum = 0;
    cur_neuron->transfer = &tanh;

    // Init first weight. It's for the bias and therefore has no belonging neuron.
    weight_head cur_weight_head = create_weight_node (randfrom(-0.5, 0.5), NULL);
    weight_head weights = cur_weight_head, prev_weight_head = NULL;

    unsigned int first_index = input_neuron_head->index;

	// Loop through input neurons and create weights for every neuron.
	do {
		// Initialize the new current weight randomly, point it to the current neuron.
		cur_weight_head = create_weight_node (randfrom(-0.5, 0.5), input_neuron_head->current_neuron);

		// If we have a previous weight node, point it to the current weight node
		if (NULL != prev_weight_head) {
			prev_weight_head->next = cur_weight_head;
		}

		// Move to next element in input list.
		prev_weight_head = cur_weight_head;
		if (NULL != input_neuron_head->next) input_neuron_head = input_neuron_head->next;
	} while (input_neuron_head->index != first_index);

    cur_neuron->weights		  = weights;

    return cur_neuron;
}

/**
 *	Free a given neuron.
 */
void free_neuron (pNeuron cur_neuron) {
	if (NULL == cur_neuron) return;

	free_weight_list (cur_neuron->weights);

    free (cur_neuron);
}

/**
 *  Function to calculate the sum in a neuron given a list of inputs
 *  and their weights, where the first weight is the bias' weight.
 */
int neuron_sum (pNeuron cur_neuron) {
	if (NULL == cur_neuron || NULL == cur_neuron->weights) return EXIT_FAILURE;

	cur_neuron->net_sum = 0;
	weight_head cur_weight_head = cur_neuron->weights;

	while (NULL != cur_weight_head) {
		// Add weight * (input neuron output) to the net sum. If the input neuron for the weight is not set it's the bias-weight.
		cur_neuron->net_sum += cur_weight_head->weight->value * (cur_weight_head->weight->from ? cur_weight_head->weight->from->output : 1);

		cur_weight_head = cur_weight_head->next;
	}

	return EXIT_SUCCESS;
}

int neuron_calc (pNeuron cur_neuron) {
	if (NULL == cur_neuron || NULL == cur_neuron->weights) return EXIT_FAILURE;

	neuron_sum (cur_neuron);
	cur_neuron->output  = cur_neuron->transfer (cur_neuron->net_sum);

	return EXIT_SUCCESS;
}

/*int neuron_calc_deltas (pNeuron cur_neuron, double optimal_output) {
	if (NULL == cur_neuron) return 1;

	list_head cur_weight = cur_neuron->weights;
	list_head cur_delta  = cur_neuron->weight_deltas;
	while (NULL != cur_weight && NULL != cur_delta) {
		cur_delta->value = cur_weight->value - cur_weight->value; //TODO: Adjust.
		cur_delta  = cur_delta->next;
		cur_weight = cur_weight->next;
	}

	return 0;
}*/

int neuron_apply_deltas (pNeuron cur_neuron) {
	if (NULL == cur_neuron || NULL == cur_neuron->weights) return EXIT_FAILURE;

	weight_head cur_weight = cur_neuron->weights;

	while (NULL != cur_weight) {
		cur_weight->weight->value += cur_weight->weight->delta;
	}

	return 0;
}

