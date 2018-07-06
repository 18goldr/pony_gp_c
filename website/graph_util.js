function set_fitnesses() {
    let f = get_fitnesses();

    let ctx = document.getElementById("performance").getContext("2d");

    let num_generations = f.length;

    let data_labels = [];

    for (let i=0; i < num_generations; i++) {
        data_labels.push("Gen " + i);
    }

    let pchart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: data_labels,

            datasets: [{
                label: "Fitness",
                data: f,
                borderColor: 'rgb(81, 145, 255)',
                backgroundColor: 'rgba(138, 178, 252, 0.51)'
            }]
        },
        options: {
            title: {
                display: true,
                text: "Generation vs. Fitness",
                position: 'bottom'
            }
        }
    });
}

function get_fitnesses() {
    let fitnesses = [];
    let output = localStorage.wasm_output;
    let gen_i = -1;
    while ((gen_i = output.indexOf("Generation: ", gen_i + 1)) != -1) {
        let str = "Fitness: ";
        let start_i = output.indexOf(str, gen_i) + str.length;
        let end_i = output.indexOf("\n", start_i);

        fitnesses.push(output.substring(start_i, end_i));
    }

    return fitnesses;
}