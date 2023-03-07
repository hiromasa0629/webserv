/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:21:21 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 23:55:23 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpResponse.hpp"

TmpResponse::TmpResponse(void) {}

TmpResponse::~TmpResponse(void) {}

TmpResponse::TmpResponse(const TmpRequest& req, const ServerConfig& sconfig, char** envp)
	: _req(req), _envp(envp)
{

}
