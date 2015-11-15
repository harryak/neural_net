#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "neuron.h"

typedef struct layer {
	unsigned int index;
	unsigned int neuron_count;

	neuron_head neurons;
} *pLayer;

/**
 * A MLP is a doubly linked list of layers.
 */
typedef struct mlp {
	unsigned int layer_count;

	pLayer current_layer;
	struct mlp *prev;
	struct mlp *next;
} *mlp;

/**
 *	Inits a layer-struct.
 */
pLayer create_layer (unsigned int index, unsigned int layer_size, unsigned int prev_layer_size) {
	pLayer cur_layer = (pLayer) malloc (sizeof(struct layer));

	cur_layer->index		= index;
	cur_layer->neuron_count = layer_size;
	cur_layer->neurons		= create_neuron_list (layer_size, prev_layer_size);

	return cur_layer;
}

/**
 *	Frees a layer-struct.
 */
void free_layer (pLayer cur_layer) {
	if (NULL == cur_layer) return;
	if (NULL == cur_layer->neurons) {
		free (cur_layer);
		return;
	}

	free_neuron_list (cur_layer->neurons);
	free (cur_layer);
}

int main (int argc, char *argv[], char *env[]) {
	// First check the parameters.
	if (argc < 3) {
		printf ("Usage: \"%s\" <layout file> <training file>.\n", argv[0]);
		return EXIT_FAILURE;
	}
	printf ("Using layout file %s and training file %s.\n", argv[1], argv[2]);

	FILE *p_trainig_file;
	FILE *p_layout_file;

	p_layout_file = fopen (argv[1], "r");
	p_trainig_file = fopen (argv[2], "r");
	if (p_layout_file == NULL) {
		printf ("The file %s does not exist or is not readable.\n", argv[1]);
		return EXIT_FAILURE;
	}
	if (p_trainig_file == NULL) {
		printf ("The file %s does not exist or is not readable.\n", argv[2]);
		return EXIT_FAILURE;
	}
	fclose(p_layout_file);
	fclose(p_trainig_file);

	// Create MLP.
	mlp mlp_instance = (mlp) malloc (sizeof(struct mlp));
	// Somehow get number of layers with their neuron count and input count.
	unsigned int input_count  = 101;
	unsigned int output_count = 30;
	unsigned int layers = 2, i = 1;
	//list_node layer_counts = init_list (6); //Only needed if we have hidden layers.

	// Init the MLP. It is a doubly linked list of layers, leaving out the input layer.
	mlp_instance->layer_count = layers - 1; // The input layer is no real neuron-layer.
	mlp_instance->current_layer = NULL;
	mlp_instance->next = mlp_instance;
	mlp_instance->prev = mlp_instance;

	// Init hidden layers.
	/*while (i < layers - 1) {
		mlp_instance->current_layer = init_layer (i, layer_counts, input_count);
	}*/

	// Init output layer.
	mlp_instance->current_layer = create_layer (i, output_count, input_count);

	// Do main stuff.

	// Free everything.
	free_layer (mlp_instance->current_layer);
	free (mlp_instance);

    return EXIT_SUCCESS;
}
