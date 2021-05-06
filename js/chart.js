const algo_names_elems = document.getElementsByClassName("algo_name");
const algo_times_elems = document.getElementsByClassName("algo_time");

var algo_names = [], algo_times = [];

// extracting values from form -> c program -> results.cgi
for (let i = 0; i < algo_names_elems.length; i++) {
    algo_names.push(algo_names_elems[i].innerHTML);
    algo_times.push(algo_times_elems[i].innerHTML);
}

// for proper scaling of y axis values
const min_time = Math.min(algo_times[0], algo_times[1], algo_times[2]);
const max_time = Math.max(algo_times[0], algo_times[1], algo_times[2]);

am4core.ready(function() {

    // Themes begin
    am4core.useTheme(am4themes_animated);
    // Themes end

    // Create chart instance
    var chart = am4core.create("res-container", am4charts.XYChart);

    // legend
    chart.legend = new am4charts.Legend();

    // Add data
    chart.data = [{
        "algo": algo_names[0],
        "time": algo_times[0]
    }, {
        "algo": algo_names[1],
        "time": algo_times[1]
    }, {
        "algo": algo_names[2],
        "time": algo_times[2]
    }, {
        "algo": algo_names[3],
        "time": algo_times[3]
    }];

    // Create axes
    var categoryAxis = chart.xAxes.push(new am4charts.CategoryAxis());
    categoryAxis.dataFields.category = "algo";
    categoryAxis.renderer.grid.template.location = 0;
    categoryAxis.renderer.minGridDistance = 40;

    categoryAxis.renderer.labels.template.adapter.add("dy", function(dy, target) {
    if (target.dataItem && target.dataItem.index & 2 == 2) {
        return dy + 25;
    }
        return dy;
    });

    var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());
    valueAxis.min = min_time - (.35 * min_time);
    valueAxis.max = max_time + (.075 * max_time);
    valueAxis.stricMinMax = true;
    valueAxis.title.text = "Time (seconds)";

    // Create series
    var series = chart.series.push(new am4charts.ColumnSeries());
    series.dataFields.valueY = "time";
    series.dataFields.categoryX = "algo";
    series.name = "CPU Time";
    series.columns.template.tooltipText = "{categoryX}: [bold]{valueY}[/]";
    series.columns.template.fillOpacity = .8;


    var columnTemplate = series.columns.template;
    columnTemplate.strokeWidth = 2;
    columnTemplate.strokeOpacity = 1;

    chart.legend.position = "top";

}); // end am4core.ready()
