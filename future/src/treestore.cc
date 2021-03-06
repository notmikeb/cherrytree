/*
 * treestore.cc
 * 
 * Copyright 2017 giuspen <giuspen@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <glibmm/i18n.h>
#include <iostream>
#include "treestore.h"
#include "ct_doc_rw.h"


TheTreeStore::TheTreeStore()
{
    mr_treestore = Gtk::TreeStore::create(m_columns);
}


TheTreeStore::~TheTreeStore()
{
}


void TheTreeStore::view_connect(Gtk::TreeView *p_treeview)
{
    p_treeview->set_model(mr_treestore);
}


void TheTreeStore::view_append_columns(Gtk::TreeView *p_treeview)
{
    p_treeview->append_column("", m_columns.m_col_node_name);
}


bool TheTreeStore::read_nodes_from_filepath(Glib::ustring &filepath, Gtk::TreeIter *p_parent_iter)
{
    bool ret_ok = false;
    CherryTreeDocRead *p_ct_doc_read = nullptr;
    if(Glib::str_has_suffix(filepath, "ctd"))
    {
        p_ct_doc_read = new CherryTreeXMLRead(filepath);
    }
    else if(Glib::str_has_suffix(filepath, "ctb"))
    {
        p_ct_doc_read = new CherryTreeSQLiteRead(filepath);
    }
    if(p_ct_doc_read != nullptr)
    {
        p_ct_doc_read->m_signal_add_bookmark.connect(sigc::mem_fun(this, &TheTreeStore::on_request_add_bookmark));
        p_ct_doc_read->m_signal_append_node.connect(sigc::mem_fun(this, &TheTreeStore::on_request_append_node));
        p_ct_doc_read->tree_walk(p_parent_iter);
        delete p_ct_doc_read;
        ret_ok = true;
    }
    return ret_ok;
}


Gtk::TreeIter TheTreeStore::append_node(t_ct_node_data *p_node_data, Gtk::TreeIter *p_parent_iter)
{
    Gtk::TreeIter new_iter;
    std::cout << p_node_data->name << std::endl;

    if(p_parent_iter == nullptr)
    {
        new_iter = mr_treestore->append();
    }
    else
    {
        new_iter = mr_treestore->append(static_cast<Gtk::TreeRow>(**p_parent_iter).children());
    }
    Gtk::TreeRow row = *new_iter;
    //row[m_columns.m_col_icon_stock_id] = ;
    row[m_columns.m_col_node_name] = p_node_data->name;
    //row[m_columns.m_col_text_buffer] = ;
    row[m_columns.m_col_node_unique_id] = p_node_data->node_id;
    row[m_columns.m_col_syntax_highlighting] = p_node_data->syntax;
    //row[m_columns.m_col_node_sequence] = ;
    row[m_columns.m_col_node_tags] = p_node_data->tags;
    row[m_columns.m_col_node_ro] = p_node_data->is_ro;
    //row[m_columns.m_col_aux_icon_stock_id] = ;
    row[m_columns.m_col_custom_icon_id] = p_node_data->custom_icon_id;
    row[m_columns.m_col_weight] = _get_pango_weight(p_node_data->is_bold);
    //row[m_columns.m_col_foreground] = ;
    row[m_columns.m_col_ts_creation] = p_node_data->ts_creation;
    row[m_columns.m_col_ts_lastsave] = p_node_data->ts_lastsave;
    return new_iter;
}


void TheTreeStore::on_request_add_bookmark(gint64 node_id)
{
    m_bookmarks.push_back(node_id);
}


guint16 TheTreeStore::_get_pango_weight(bool is_bold)
{
    return is_bold ? PANGO_WEIGHT_HEAVY : PANGO_WEIGHT_NORMAL;
}


Gtk::TreeIter TheTreeStore::on_request_append_node(t_ct_node_data *p_node_data, Gtk::TreeIter *p_parent_iter)
{
    return append_node(p_node_data, p_parent_iter);
}
