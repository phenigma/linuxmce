<?
class Navigation
{
	var $levels;
	var $rootItem;
	var $keys;
	var $selectedItem = '';
	var $logo_img_file = 'logo.gif';
	var $logo_img_file_w = 119;
	var $logo_img_file_h = 42;
	
	function Navigation()
	{
		$this->levels = array();
		$this->keys = array();
		$this->rootItem =& new NavItem();
		$this->rootItem->root =& $this;
		$this->rootItem->level = -1;
	}
	
	/**
	* @return NavItem
	* @param str $key
	* @param str $title
	* @param str $description
	* @param str $url
	* @param bool $reselectable
	* @param str $target
	*/
	function & addItem($key, $title, $description, $url, $reselectable=false, $target='_top')
	{
		return $this->rootItem->addItem($key, $title, $description, $url, $reselectable, $target);
	}
	
	function selectKey($key)
	{
		$this->selectedItem =& $this->keys[$key];
		if ($this->selectedItem) $this->selectedItem->select();
		return (bool)$this->selectedItem;
	}
	
	function & get_item($key)
	{
		return $this->keys[$key];
	}
	
	function layout_head($path, $loginName='')
	{
		// $path/default.php
		if ($path !== '') $path .= '/';
		if ($loginName==='' && isset($_SESSION['username'])) $loginName = $_SESSION['username'];
		?>
		<table width="100%" height="100%" cellpadding="0" cellspacing="0" border="0" style="background-color:#ffffff;">
			<tr valign="top"><td>
				<table width="100%" height="100%" cellpadding="0" cellspacing="0" border="0" style="background-color:#ffffff;"><tr valign="top">
					<td width="30%">&nbsp;</td>
					<td align="center" width="40%">
						<a href="/<?= $path ?>" target="_top"><img src="/<?= $path ?>inc/images/<?= $this->logo_img_file ?>" width="<?= $this->logo_img_file_w ?>" height="<?= $this->logo_img_file_h ?>" border="0"></a>
					</td>
					<td width="30%" align="right">Logged in as: <b><?= $loginName ?></b></td>
				</tr></table>
			</td></tr>
		<?

		$navItem =& $this->rootItem; /* @var $navItem NavItem */
		$color = "black";
		$class = "nav1LightLink";

		while (true)
		{
			?>
			<tr valign="top" bgcolor="<?= $color ?>"><td><img src="/<?= $path ?>inc/images/pixel_blank.gif" width="4" height="4"></td></tr>
			<tr valign="top" bgcolor="<?= $color ?>"><td><table cellpadding="0" cellspacing="0" border="0"><tr>
			<td><img src="/<?= $path ?>inc/images/pixel_blank.gif" width="<?= (50 + 30*$navItem->level) ?>" height="20"></td>
			<?
			foreach ($navItem->items as $item)
			{
				?><td bgcolor="<?= $item->selected ? $item->selectedColor : $color ?>" nowrap="nowrap">&nbsp;&nbsp;&nbsp;<?
				
				if ($item->selected)
				{
					$colorSelected = $item->selectedColor;
					$classSelected = ($item->selectedTextColor == "dark") ? "nav1Dark" : "nav1Light";
					$classSelectedLink = $classSelected.'Link';
					
					if ($item->reselectable)
					{
						?><a href="<?=$item->url?>" class="<?=$classSelectedLink?>" target="<?=$item->target?>" title="<?=$item->description?>"><?= $item->title ?></a><?
					}
					else 
					{
						?><span class="<?= $classSelected ?>"><?= $item->title ?></span><?
					}
				}
				else
				{
					?><a href="<?= $item->url ?>" class="<?= $class ?>" target="<?= $item->target ?>" title="<?= $item->description ?>"><?= $item->title ?></a><?
				}
				?> &nbsp; </td><?
			}
			
			?></tr></table></td></tr><?
			
			if (isset($navItem->selectedChild) && count($navItem->selectedChild->items))
			{
				$navItem =& $navItem->selectedChild;
				$color = $colorSelected;
				$class = $classSelectedLink;
			}
			else break;
		}
		?><tr height="100%"><td valign="top"><table cellpadding="5" width="100%"><tr><td valign="top" align="left"><?
	}
	
	
	
	function layout_foot()
	{
		?></td></tr></table></td></tr></table><?
	}
}

class NavItem
{
	var $level;
	var $root;	//points to the main Navigation object
	var $parent;
	var $items;
	var $key;
	var $title;
	var $description;
	var $url;
	var $selected;
	var $selectedChild;		//can only be set when this item is selected
	var $reselectable;
	var $selectedColor;
	var $selectedTextColor;		// "light" or "dark";
	var $target;
	
	function NavItem()
	{
		$this->selectedColor = "white";
		$this->selectedTextColor = "dark";
	}
	
	
	/**
	* @return NavItem
	* @param str $key
	* @param str $title
	* @param str $description
	* @param str $url
	* @param bool $reselectable
	* @param str $target
	* @desc Add a sub item
	*/
	function & addItem($key, $title, $description, $url, $reselectable=false, $target='_top')
	{
		$newNavItem =& new NavItem();
		$newNavItem->root =& $this->root;
		$newNavItem->level = $this->level+1;
		$newNavItem->parent =& $this;
		$nawNavItem->key = $key;
		$newNavItem->root->keys[$key] =& $newNavItem;
		$newNavItem->title = $title;
		$newNavItem->description = $description;
		$newNavItem->url = $url;
		$newNavItem->selected = false;
		$newNavItem->reselectable = $reselectable;
		$newNavItem->target = $target;
		$this->items[] =& $newNavItem;
		return $newNavItem;
	}
	
	function select()
	{
		$this->selected = true;
		if (isset($this->parent))
		{
			$this->parent->select();
			$this->parent->selectedChild =& $this;
		}
	}
}