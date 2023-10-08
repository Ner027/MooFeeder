UPDATE control_box_data SET (<session_token>,
                             <name>,
                             <password_salt>,
                             <username>,
                             <password>) = (?, ?, ?, ?, ?) WHERE <id> = ?;
