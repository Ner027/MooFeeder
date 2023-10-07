UPDATE control_box_data SET (<nof_connections>,
                             <session_token>,
                             <name>,
                             <password_salt>,
                             <username>,
                             <password>) = (?, ?, ?, ?, ?, ?) WHERE <id> = ?;
