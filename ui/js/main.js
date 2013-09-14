/* main.js
 * Heiher <root@heiher.info>
 */

function get_base_url () {
	return window.location.protocol + '//' +
		window.location.hostname;
}

function load_file_list () {
	$.ajax({ url: '/fb', type: 'GET',
		success: function (data) {
			var content = '';
			var files = data.split ('\r\n');

			content += "<table border='0'>";
			for (var i=0; i<files.length; i++) {
				if ('' == files[i]) continue;
				content += "<tr>";
				content += "<th><a style='cursor:pointer' " +
							"title='点击查看此文件的详细信息' " +
							"onclick=\"query_file_info ('" + files[i] + "')\">" +
							files[i] + "</a></th>";
				content += "<td> &#10132;&nbsp; </td>";
				content += "<td><a target='_blank' href='/fb/" + files[i] + "'>" +
							get_base_url () + "/fb/" + files[i] + "</a></td>";
				content += "</tr>";
			}
			content += "</table>";

			$('#file-list').html (content);

			setTimeout (load_file_list, 2000);
		}});
}

function query_file_info (file) {
	$.ajax({ url: '/fb/query?file=' + file, type: 'GET',
		success: function (data) {
			alert (data);
	}});
}

$('#file-upload').submit (function () {
	$(this).ajaxSubmit ({
		resetForm: true,
		beforeSend: function () {
			$('#submit').prop ('disabled', true);
			$('#submit').val ('正在上传 0%...');
		},
		uploadProgress: function(event, position, total, percent) {
			$('#submit').val ('正在上传 ' + percent + '%...');
		},
		success: function () {
			$('#submit').val ('正在上传 99%...');
		},
		complete: function (xhr) {
			$('#submit').prop ('disabled', false);
			$('#submit').val ('开始上传');
			alert (xhr.statusText);
		}
	});

	return false;
});

