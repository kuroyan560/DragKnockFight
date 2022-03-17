#pragma once

// 当たり判定にてどの辺と当たったかの識別用
enum INTERSECTED_LINE {
	INTERSECTED_NONE,	// どこの壁にも当たっていない。
	INTERSECTED_TOP,	// 上に当たっている。
	INTERSECTED_RIGHT,	// 右に当たっている。
	INTERSECTED_LEFT,	// 左に当たっている。
	INTERSECTED_BOTTOM,	// 下に当たっている。
	INTERSECTED_RISE,	// のし上がる位置に当たっている。
};