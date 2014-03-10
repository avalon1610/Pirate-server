var controlref = {'def-env':'env-content','def-scan':'tcp-content','tcp-scan':'tcp-content',
  	                  'udp-scan':'udp-content','def-test':'arp-content','arp-test':'arp-content',
  	                  'icmp-test':'icmp-content','def-runner':'runner-content'};

var data = {
		labels : [],
		datasets : [
			{
				fillColor : "rgba(220,220,220,0.5)",
				strokeColor : "rgba(220,220,220,1)",
				pointColor : "rgba(220,220,220,1)",
				pointStrokeColor : "#fff",
				data : []
			}
		]
	};
function showChart(ctx,x,y) {
	if (data.labels.length > 50) {
		data.labels.shift();
		data.datasets[0].data.shift();
	}
	data.labels.push(x);
	data.datasets[0].data.push(y);

	new Chart(ctx).Line(data,{animation:false});
}

function toggle_aria(obj) {
	var bar = obj.parents('tr').find('.progress');
	if (obj.hasClass('btn-play')) {
		bar.addClass('progress-striped');
		bar.addClass('active');
	} else {
		bar.removeClass('active');
		bar.removeClass('progress-striped');
	}
}

function toggle_it(obj) {
	obj.siblings().removeClass('disabled');
	obj.siblings('.btn-play').addClass('btn-success');
	obj.siblings('.btn-pause').addClass('btn-warning');
	obj.siblings('.btn-stop').addClass('btn-danger');

	if (obj.hasClass('btn-play')) {
		obj.addClass('disabled');
		obj.removeClass('btn-success');
	} else if (obj.hasClass('btn-pause')) {
		obj.addClass('disabled');
		obj.removeClass('btn-warning');
	} else if (obj.hasClass('btn-stop')) {
		obj.addClass('disabled');
		obj.removeClass('btn-danger');
		obj.siblings('.btn-pause').removeClass('btn-warning');
		obj.siblings('.btn-pause').addClass('disabled');
	}
	toggle_aria(obj);
}

var progress = {};
function BindProgressEvent() {
	$('#runner-content button').click(function(){
		var progress_bar = $(this).parents('tr').find('.progress-bar');
		var current_perc = parseInt(progress_bar.text().replace('%',''));
		var id = $(this).parents('tr').find('.mission-id').text();
		var id_obj = $(this).parents('tr').find('.mission-id');

		if ($(this).hasClass('btn-play')) {
			var perc = progress_bar.attr('data-percentage');
			progress[id] = setInterval(function(){
				if (current_perc >= perc)
					clearInterval(progress[id]);
				else {
					current_perc += 1;
					progress_bar.css('width',(current_perc)+'%');
				}

				progress_bar.text((current_perc)+'%');
				progress_bar.attr('aria-valuenow',current_perc);
			},3000);
			toggle_it($(this));

			id_obj.addClass('running_committing');
		} else if ($(this).hasClass('btn-pause')) {
			if (progress[id] != undefined) {
				clearInterval(progress[id]);
				toggle_it($(this));

				id_obj.addClass('pause_committing');
			}
		} else if ($(this).hasClass('btn-stop')) {
			if (progress[id] != undefined) {
				clearInterval(progress[id]);
				progress_bar.css('width','0%');
				progress_bar.text('0%');
				progress_bar.attr('aria-valuenow',0);
				toggle_it($(this));

				id_obj.addClass('stop_committing');
			}
		}

		setTimeout(save_config(false),500);
	});
}

$(function(){
	$('.navbar a').hover(function(){
		if ($(this).parent().parent().hasClass('dropdown-menu')==false)
			$('.dropdown-menu').slideUp(100);
	},function(){});
	$('.navbar .dropdown-menu').hover(function(){},function(){
		$(this).slideUp(100);
	});

	$('.navbar .dropdown-toggle').hover(function(){
		$(this).next('.dropdown-menu').slideDown(100);
	},function(){});

	$('.navbar a').click(function(){
		if ($(this).parent().hasClass('active'))
	    	return;
		else {
			for (var i in controlref) {
				if($(this).attr('id') == i) {
					auto_save_config();
					$('.present').hide('slide',{ direction: "left" });
					$('.present').removeClass('present');
					$('#'+controlref[i]).show('slide',{ direction: "left" });
					$('#'+controlref[i]).addClass('present');
					$('.navbar .active').removeClass('active');
					if ($(this).parents('.dropdown-menu').length>0) {
						$(this).parent().addClass('active');
						$(this).parents('.dropdown').addClass('active');
					} else {  
						if ($(this).next('ul').find('a').length>0)
						$(this).next('ul').find('a').parent()[0].className+='active';
						$(this).parent().addClass('active');
					}
					break;
				}
			}
		}
	});

	$('.list-group-item').click(function(){
		if (!$(this).children('.list-group-item-text').is(':visible'))
		{
			$('.list-group-item-text').slideUp(200);
			$('.list-group-item').removeClass('sub-present');
		}
		$(this).children('.list-group-item-text').slideDown(200);
		$(this).addClass('sub-present');

		var ref = '.content-info[name='+$(this).attr('name')+']';
		if (!$(ref).is(':visible'))
			$('.content-info').slideUp(200);
		$(ref).slideDown(200);
	});

	$('#rate-limit-group label').click(function(){
		var id = $(this).children('input').attr('id');
		if (id == 'Limit-radio-Rate')
		{
			$("#dos-input").slideUp(200);
			$("#limit-input").slideDown(200);	
		}
		else if (id == 'Dos-radio-Rate')
		{
			$("#limit-input").slideUp(200);
			$("#dos-input").slideDown(200);
		}
		else
		{
			$("#dos-input").slideUp(200);
			$("#limit-input").slideUp(200);
		}
	});

	$('#duration-group label').click(function(){
		var id = $(this).children('input').attr('id');
		if (id == 'Set-radio-Duration')
			$('#duration-input').slideDown(200);
		else
			$('#duration-input').slideUp(200);
	});

	$('label').tooltip();

	$('#auto-save-checkbox').on('switch-change',function(){
		if (!$('#auto-save-checkbox').bootstrapSwitch('status'))
			$('#manual-save-button').fadeIn(100);
		else
		{
			$('#manual-save-button').fadeOut(100);		
		}
	});

	$('input').on('keyup blur',destroy_popover);

	BindProgressEvent();

	ctx = $('#myChart').get(0).getContext('2d');
	
	$('#myModal').modal('show',{
  		keyboard: false
	});

	$('#myModal').on('shown.bs.modal',function(e){
		$('.modal').off('click');

		$('#myModal button').on('click',function(e){
			$('body').html('<div style="position:absolute;\
			top:50%;left:50%;margin-left:-80px;margin-top:-25px;\
			width:160px;height:50px;"\
			<h4 >System Initialize Failed!</h4></div>');
		});

		init_mission();
	});
	
});
